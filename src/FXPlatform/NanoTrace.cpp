#include "Directory.h"
#include "FailFast.h"
#include "Logger.h"
#include "NanoTrace.h"
#include <iomanip>
#include <fstream>
#include <algorithm>
//#include "boost/algorithm/string.hpp"

DEFINE_ENUM(SystemTraceType, SYSTEM_TRACE_TYPE);

NanoTrace NanoTrace::m_nanoTrace(3000);
const char *NanoTrace::m_replaceTokens[] = { "{0}", "{1}", "{2}", "{3}", "{4}", "{5}", "{6}", "{7}", "{8}", "{9}" };

void NanoTraceTiming::AddTiming(double value)
{
    // If we haven't hit the limit yet, just add the value
    if(count < maxItems)
    {
        values[count] = value;
    }
    else
    {
        // subtract off the oldest value first
        int oldestValueIndex = count % maxItems;
        total -= values[oldestValueIndex];
        values[oldestValueIndex] = value;
    }
    
    if(value > maxValue) { maxValue = value; }
    if(value < minValue) { minValue = value; }
    if(highThreshold != -1 && value > highThreshold)
    {
        highThresholdCount++;
    }
    
    total += value;
    runningTotal += value;
    count++;
}

NanoTrace::NanoTrace(int size) :
    m_allowedTraceType((unsigned long)SystemTraceType::All),
    m_debugTraceType((unsigned long)SystemTraceType::All),
    m_detailLevel(TraceDetail::Normal),
	m_lastTime(0),
    m_traceRecordsWritten(0),
    m_maxTraceRecords(size),
	m_timings(shared_ptr<TimingMap>(new TimingMap())),
    m_traceRecords(vector<TraceRecord>(size)),
    m_traceRecordsSize(size)
{
	m_stopWatch.startTimer();
}

void NanoTrace::ClearTraces()
{
    lock_guard<recursive_mutex> lock(traceMutex);
	m_timings->clear();
    m_traceRecordsWritten = 0;
}

void NanoTrace::Close()
{
    lock_guard<recursive_mutex> lock(traceMutex);

    if(m_fileStream != nullptr)
    {
        m_fileStream->close();
        m_fileStream = nullptr;
    }
}

void NanoTrace::CopyLogToFile(unsigned long traceType, const string &pathAndFile)
{
    lock_guard<recursive_mutex> lock(traceMutex);

    shared_ptr<ofstream> fileStream;
    if(pathAndFile.size() > 0)
    {
        fileStream = shared_ptr<ofstream>(new ofstream());
        fileStream->open(pathAndFile, ios::binary);
    }

    CopyLogToStream("FileCopy", traceType, fileStream);
}

void NanoTrace::CopyLogToStream(const string &key, unsigned long traceType, shared_ptr<ostream> stream)
{
    lock_guard<recursive_mutex> lock(traceMutex);

    TraceCollectorsType::iterator found = m_traceCollectors.find(key);
    if(found != m_traceCollectors.end())
    {
        m_traceCollectors.erase(found);
    }
    
    if(stream != nullptr)
    {
        m_traceCollectors[key] = pair<unsigned long, shared_ptr<ostream>>(traceType, stream);
    }
}

shared_ptr<ostream> NanoTrace::GetLogCopy(const string &key)
{
    lock_guard<recursive_mutex> lock(traceMutex);

    return m_traceCollectors[key].second;
}

shared_ptr<NanoTrace::TimingMap> NanoTrace::DetachTimings()
{
    lock_guard<recursive_mutex> lock(traceMutex);

	shared_ptr<NanoTrace::TimingMap> timings = m_timings;
	m_timings = shared_ptr<NanoTrace::TimingMap>(new TimingMap());

	return timings;
} 

void NanoTrace::FormatTrace(TraceRecord &record, ostream &stream)
{
    string substitutedString = record.traceKey();
    
    // Write the timestamp
    char buffer[20];
    time_t timestamp = record.timestamp();
    strftime(buffer, 20, "%m%d %H:%M:%S ", localtime(&timestamp));
    stream << buffer;
    
    if(record.isTiming())
    {
        stream << "=" << std::fixed << std::setw(6) << std::setprecision(4)
        << std::setfill(' ') << record.elapsedTime() << " ";
    }
    else
    {
        stream << "^" << std::fixed << std::setw(6) << std::setprecision(4)
        << std::setfill(' ') << record.elapsedTime() << " ";
    }
    
    // Fill in the traceKey with any data, anything not used should be tacked onto the end
    bool hasLeftoverData = false;
    for(int dataIndex = 0; dataIndex < record.dataCount(); ++dataIndex)
    {
        if(!ReplaceAll(substitutedString, m_replaceTokens[dataIndex], record.data()[dataIndex]))
        {
            hasLeftoverData = true;
        }
    }

    stream << substitutedString;
    if(hasLeftoverData)
    {
        stream << ": ";
        for(int dataIndex = 0; dataIndex < record.dataCount(); ++dataIndex)
        {
            stream << record.data()[dataIndex] << ", ";
        }
    }

    stream << "\r\n";
}

string NanoTrace::GetTimingString(const string &traceKey)
{
    lock_guard<recursive_mutex> lock(traceMutex);
	return GetTimingString(traceKey, (*m_timings)[traceKey]);
}

string NanoTrace::GetTimingString(const string &traceKey, const NanoTraceTiming &timing)
{
	stringstream stream;
	stream.precision(6);
	stream.setf(ios::fixed,ios::floatfield);   

    stream << traceKey <<
    ": Ave=" << timing.AverageTime() <<
    ", Max=" << timing.maxValue;
	
	if(timing.highThreshold != -1)
	{
		stream << ", # > " << timing.highThreshold << "=" << timing.highThresholdCount;
	}

    stream <<
    ", Min=" << timing.minValue <<
    ", #=" << (int) timing.count <<
    ", RunningTotal =" << timing.runningTotal;

	stream << "\r\n";
	return stream.str();
}

// LogToFile sets the name of the master log file.
// When called, it gathers up all the fragment log files that were previously created and puts them into the master log file
// Logging actually begins in a new file that has an integer appended onto it.  We only reconsolidate into the log file specified
// to this call (filePath) when you call it *again*.
// This is because it is called at startup and any crashes want the consolidated file from the previous run
void NanoTrace::LogToFile(const string &filePath, uint32_t maxFileCount)
{
    lock_guard<recursive_mutex> lock(traceMutex);

    Directory::SplitPath(filePath, m_path, m_fileWithoutExtension, m_fileExtension);
    m_directory = shared_ptr<Directory>(new Directory(m_path, false));
    m_fileIndex = 0;
    m_maxFileCount = maxFileCount;
    
    // If there are existing fragment files, they are consolidated
    if(m_fileStream != nullptr)
    {
        m_fileStream->close();
    }
    
    // Always reset to zero so that we will create a new fragment file when we next call Trace
    // Otherwise, if some logging happened before LogToFile we will have a non-zero value here and
    // TraceImpl won't call OpenNextFragmentFile because it assumes someone else already created it
    m_traceRecordsWritten = 0;

    ConsolidateFragmentFiles(filePath);
}

void NanoTrace::ConsolidateFragmentFiles(const string &pathAndFile)
{
    string path;
    string fileWithoutExtension;
    string fileExtension;
    Directory::SplitPath(pathAndFile, path, fileWithoutExtension, fileExtension);
    Directory directory(path);
    
    // If the consolidated file already exists, it is deleted
    string consolidatedFile = Directory::Combine(path, fileWithoutExtension + "." + fileExtension);
    if(Directory::FileExists(consolidatedFile))
    {
        Directory::DeleteItem(consolidatedFile);
    }
    
    // If fragment files exist, they are concatenated into a new consolidated file and deleted
    shared_ptr<vector<string>> filenames = directory.GetFileNames(fileWithoutExtension + "*." + fileExtension, false, true);
    if(filenames->size() > 0)
    {
        shared_ptr<FileStream> consolidatedStream = shared_ptr<FileStream>(new FileStream());
        consolidatedStream->Open(consolidatedFile, FileOpenStyle::CreateAlways, AccessRights::ReadWrite);
        string userID = "ID: " + GetUniqueDeviceID() + "\r\n";
        consolidatedStream->Write(userID);
        string buildInfo = "Build: " + lexical_cast<string>(GetBuild()) + "\r\n";
        consolidatedStream->Write(buildInfo);

        std::sort(filenames->begin(), filenames->end());
        vector<uint8_t> buffer;
        uint32_t bufferSize = 2048;
        buffer.resize(bufferSize);
        for(vector<string>::iterator iter = filenames->begin(); iter != filenames->end(); ++iter)
        {
            // Open the fragment
            shared_ptr<FileStream> fragmentStream = directory.OpenFile(*iter, FileOpenStyle::OpenExisting, AccessRights::Read);
            
            // Append onto consolidated stream
            uint32_t bytesRead;
            do
            {
                bytesRead = fragmentStream->Read(buffer, bufferSize);
                consolidatedStream->Write(buffer, bytesRead);
            } while(bytesRead == bufferSize);
            
            // Delete the fragment
            fragmentStream->Close();
            directory.DeleteLocalFile(*iter);
        }
    }
}

// Creates a new file and sets the file pointer to it
// if we have created more than m_maxFileCount files, we delete the oldest file
void NanoTrace::OpenNextFragmentFile()
{
    if(m_fileStream != nullptr)
    {
        m_fileStream->close();
    }
    
    ++m_fileIndex;
    if(m_fileIndex > m_maxFileCount)
    {
        // Once we've passed the maxFileCount once, every new file should have an old one deleted
        m_directory->DeleteLocalFile(m_fileWithoutExtension + lexical_cast<string>(m_fileIndex - m_maxFileCount) + "." + m_fileExtension);
    }
    
    m_fileStream = shared_ptr<ofstream>(new ofstream());
    m_fileStream->open(m_directory->FullPathFromLocalPath(m_fileWithoutExtension + lexical_cast<string>(m_fileIndex) + "." + m_fileExtension), ios::binary);
}

string NanoTrace::PerformanceTraces()
{
    lock_guard<recursive_mutex> lock(traceMutex);
	stringstream stream;
	for(TimingMap::iterator iter = m_timings->begin(); iter != m_timings->end(); ++iter)
	{
		stream << GetTimingString(iter->first, iter->second);
	}

	return stream.str();
}

double NanoTrace::PerfTraceImpl(const string &traceKey, const double startTime, int traceType, const TraceDetail levelOfDetail, bool timingOnly,
                                int dataCount,
                                const char *data1, const char *data2, const char *data3, const char *data4, const char *data5,  
                                const char *data6, const char *data7, const char *data8, const char *data9, const char *data10)
{
	double result;
	FailFastAssert(startTime != -1);
	TraceImpl(traceKey, startTime, traceType, levelOfDetail, timingOnly, &result, 
                dataCount, data1, data2, data3, data4, data5, data6, data7, data8, data9, data10);

	return result;
}

void NanoTrace::ResetTimingItem(const string &traceKey)
{
    lock_guard<recursive_mutex> lock(traceMutex);

    m_timings->erase(traceKey);
}

void NanoTrace::ResetTraces()
{
    lock_guard<recursive_mutex> lock(traceMutex);
	for(TimingMap::iterator iter = m_timings->begin(); iter != m_timings->end(); ++iter)
	{
		iter->second.Reset();
	}
}

void NanoTrace::SetTimingParameters(const string &traceKey, NanoTraceTiming &parameters)
{
    lock_guard<recursive_mutex> lock(traceMutex);
	(*m_timings)[traceKey] = parameters;
}

void NanoTrace::StopCopyLogToFile()
{
    lock_guard<recursive_mutex> lock(traceMutex);

    CopyLogToFile(0, "");
}

// returns the current time at the end of the routine, useful for tracking the start of an event
// if startTime > -1, then timingResult is filled in with the difference between startTime and the time at the beginning of the routine
// this is useful for tracking the end of an event
double NanoTrace::TraceImpl(const string &traceKey, const double startTime, int traceType, const TraceDetail levelOfDetail, bool timingOnly, double *timingResult, 
                        int dataCount,
                        const char *data1, const char *data2, const char *data3, const char *data4, const char *data5,  
                        const char *data6, const char *data7, const char *data8, const char *data9, const char *data10)
{
    if(!(traceType & allowedTraceType()) || !(levelOfDetail <= detailLevel()))
    {
        return 0;
    }

    lock_guard<recursive_mutex> lock(traceMutex);
    
	// Determine timing between trace calls
	double currentTime = m_stopWatch.getCurrentTime();
	double elapsedTime;
    bool isTiming = false;

	if(startTime > -1)
	{
        isTiming = true;
		elapsedTime = currentTime - startTime;
		if(timingResult != nullptr)
		{
			*timingResult = elapsedTime;
		}

		NanoTraceTiming &item = (*m_timings)[traceKey];
        item.AddTiming(elapsedTime);
	}
	else
	{
		elapsedTime = currentTime - m_lastTime;
	}

	m_lastTime = currentTime;

    // Record the Trace results
    if(!timingOnly)
    {
        TraceRecord &record = m_traceRecords[m_traceRecordsWritten % m_maxTraceRecords];
        m_traceRecordsWritten++;

        record.dataCount(dataCount);
        record.elapsedTime(elapsedTime);
        record.isTiming(isTiming);
        record.traceKey(traceKey);
        FailFastAssert(sizeof(time_t) <= sizeof(record.timestamp()));
        record.timestamp(time(nullptr));

        switch(dataCount)
        {
        case 10:
            record.data()[9] = data10;
        case 9:
            record.data()[8] = data9;
        case 8:
            record.data()[7] = data8;
        case 7:
            record.data()[6] = data7;
        case 6:
            record.data()[5] = data6;
        case 5:
            record.data()[4] = data5;
        case 4:
            record.data()[3] = data4;
        case 3:
            record.data()[2] = data3;
        case 2:
            record.data()[1] = data2;        
        case 1:
            record.data()[0] = data1;
        }

        if(m_directory != nullptr)
        {
            // Create a new fragment file each time we roll over m_maxTraceRecords
            if(((m_traceRecordsWritten - 1) % m_maxTraceRecords) == 0)
            {
                OpenNextFragmentFile();
            }
            
            FormatTrace(record, *m_fileStream);
            m_fileStream->flush();
        }
        
        if(m_debugTraceType & traceType)
        {
            stringstream stream;
            FormatTrace(record, stream);
            DebugLogMessage(traceType, levelOfDetail, stream.str().c_str());
        }
        
        for(auto collector : m_traceCollectors)
        {
            if(collector.second.first & traceType)
            {
                FormatTrace(record, *collector.second.second);
                collector.second.second->flush();
            }
        }
    }
    
    return m_stopWatch.getCurrentTime();
}

string NanoTrace::Traces()
{
    lock_guard<recursive_mutex> lock(traceMutex);
    stringstream stream;
    stream.precision(6);
	stream.setf(ios::fixed,ios::floatfield);   

	int localPosition = m_traceRecordsWritten % m_maxTraceRecords;

	if(m_traceRecordsWritten > m_traceRecordsSize)
	{
        // We have wrapped around at least once
        for(vector<TraceRecord>::iterator iter = m_traceRecords.begin() + localPosition; iter != m_traceRecords.end(); ++iter)
        {
            FormatTrace(*iter, stream);
        }
	}

    for(vector<TraceRecord>::iterator iter = m_traceRecords.begin(); iter != m_traceRecords.begin() + localPosition; ++iter)
    {
        FormatTrace(*iter, stream);
    }

    DebugLogMessage(0, TraceDetail::Diagnostic, stream.str().c_str());

	return stream.str();
}
