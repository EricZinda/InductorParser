#pragma once
#include <cfloat>
#include <map>
#include <limits>
#include <mutex>
#include "ReflectionEnum.h"
#include "Stopwatch.h"
#include <sstream>
#include "Utilities.h"
#include <vector>

using namespace std;
class Directory;

// Types < Custom are reserved for the system
// This is a bitfield
#define SYSTEM_TRACE_TYPE(item) \
    item(SystemTraceType, None, 0) \
    item(SystemTraceType, System, 1) \
    item(SystemTraceType, Graphics, 2) \
    item(SystemTraceType, Input, 4) \
    item(SystemTraceType, BlockStore, 8) \
    item(SystemTraceType, Abilities, 16) \
    item(SystemTraceType, Parsing, 32) \
    item(SystemTraceType, ScreenMgmt, 64) \
    item(SystemTraceType, Networking, 128) \
    item(SystemTraceType, Audio, 256) \
    item(SystemTraceType, Custom, 0x00000800) \
    item(SystemTraceType, Engine, (uint64_t) SystemTraceType::Custom * 2) \
    item(SystemTraceType, UserInterface, (uint64_t) SystemTraceType::Custom * 4) \
    item(SystemTraceType, HTML, (uint64_t)SystemTraceType::Custom * 8) \
    item(SystemTraceType, HTMLLayout, (uint64_t)SystemTraceType::Custom * 16) \
    item(SystemTraceType, InternalTest, (uint64_t)SystemTraceType::Custom * 32) \
    item(SystemTraceType, Animation, (uint64_t)SystemTraceType::Custom * 64) \
    item(SystemTraceType, Market, (uint64_t)SystemTraceType::Custom * 128) \
    item(SystemTraceType, Help, (uint64_t)SystemTraceType::Custom * 256) \
    item(SystemTraceType, Memory, (uint64_t)SystemTraceType::Custom * 512) \
    item(SystemTraceType, Gestures, (uint64_t)SystemTraceType::Custom * 1024) \
    item(SystemTraceType, EngineDisplay, (uint64_t)SystemTraceType::Custom * 2048) \
    item(SystemTraceType, Solver, (uint64_t)SystemTraceType::Custom * 4096) \
    item(SystemTraceType, Unifier, (uint64_t)SystemTraceType::Custom * 8192) \
    item(SystemTraceType, Planner, (uint64_t)SystemTraceType::Custom * 16384) \
    item(SystemTraceType, All, 0x0FFFFFFF)
DECLARE_ENUM(SystemTraceType,SYSTEM_TRACE_TYPE)

enum class TraceDetail
{
	Normal,
	Detailed,
	Diagnostic
};

class NanoTraceTiming
{
public:
	NanoTraceTiming(int itemsInAverage) :
		highThreshold(-1),
        maxItems(itemsInAverage)
	{
		Reset();
        values = vector<double>(maxItems);
	}

    NanoTraceTiming() :
		highThreshold(-1),
        maxItems(60)
	{
		Reset();
        values = vector<double>(maxItems);
	}

    void AddTiming(double value);

    double AverageTime() const
    {
        
        return (double) total / (double) (count < maxItems ? count : maxItems);
    }

    void Reset()
    {
    	total = 0;
        runningTotal = 0;
    	count = 0;
    	highThresholdCount = 0;
		maxValue = DBL_MIN;
		minValue = DBL_MAX;
    }

    void SetHighThreshold(double value)
    {
    	highThreshold = value;
    }

	double total;
    double runningTotal;
	int count;
	double highThreshold;
	int highThresholdCount;
	double maxValue;
	double minValue;
    int oldestValueIndex;
    vector<double> values;

private:
    int maxItems;
};

class TraceRecord
{
public:
    TraceRecord()
    {
        m_data.resize(10);
    }

    ValueProperty(private, int, dataCount);
    Property(private, vector<string>, data);
    ValueProperty(private, double, elapsedTime);
    ValueProperty(private, bool, isTiming);
    ValueProperty(private, uint64_t, timestamp);

public:
    string &traceKey() { return m_traceKey; }
    void traceKey(const string &value) { m_traceKey = value; }

private:
    string m_traceKey;
};

#define SetTraceFilter(traceType, levelOfDetail) \
    NanoTrace::Global().allowedTraceType((int) (traceType)); \
    NanoTrace::Global().detailLevel(levelOfDetail); 
#define SetTraceDefault() \
    SetTraceFilter(SystemTraceType::All, TraceDetail::Normal); \
    NanoTrace::Global().debugTraceType((int) SystemTraceType::All);
#define AddDebugTraceType(traceType) \
    NanoTrace::Global().allowedTraceType(NanoTrace::Global().allowedTraceType() | (int) traceType); \
    NanoTrace::Global().debugTraceType(NanoTrace::Global().debugTraceType() | (int) traceType);
#define AddTraceType(traceType) NanoTrace::Global().allowedTraceType(NanoTrace::Global().allowedTraceType() | (int) traceType);
#define SetTraceLevelOfDetail(levelOfDetail) NanoTrace::Global().detailLevel(levelOfDetail);
#define SetTraceFilterOnly(traceType) NanoTrace::Global().allowedTraceType((int) (traceType));
#define SetDebugTraceType(traceType) NanoTrace::Global().debugTraceType((int) traceType);

#define ResetTiming(name) NanoTrace::Global().ResetTimingItem(#name " Timing");
#define StartTiming(name, traceType, levelOfDetail) double name = NanoTrace::Global().Trace(#name " Begin", (int) traceType, levelOfDetail);
#define StartTiming1(name, traceType, levelOfDetail, value1) double name = NanoTrace::Global().Trace(#name " Begin", (int) traceType, levelOfDetail, value1);
#define EndTiming(name, traceType, levelOfDetail) NanoTrace::Global().PerfTrace(#name " Timing", name, (int) traceType, levelOfDetail);
#define EndTimingAssert(name, maxTime, traceType, levelOfDetail) \
	name = NanoTrace::Global().PerfTrace(#name " Timing", name, (int) traceType, levelOfDetail); \
	FailFastAssert(name < maxTime);
#define StartTimingOnly(name, traceType, levelOfDetail) double name = NanoTrace::Global().TraceTimingOnly(#name " Begin", (int) traceType, levelOfDetail);
#define EndTimingOnly(name, traceType, levelOfDetail) NanoTrace::Global().PerfTraceTimingOnly(#name " Timing", name, (int) traceType, levelOfDetail);
#define EndTimingTraceIf(name, maxTime, traceType, levelOfDetail) \
	name = NanoTrace::Global().PerfTraceTimingOnly(#name " Timing", name, (int) traceType, levelOfDetail); \
	if(name > maxTime) \
    { \
        NanoTrace::Global().Trace(#name " Timing", (int) traceType, levelOfDetail, name, " > ", maxTime); \
    } \


#define EndTimingOnlyAssert(name, maxTime, traceType, levelOfDetail) \
	name = NanoTrace::Global().PerfTraceTimingOnly(#name " Timing", name, traceType, levelOfDetail); \
	FailFastAssert(name < maxTime);


#define AssertAverage(name, maxTime) \
    double name##Average = ((double) (*NanoTrace::Global().timings())[#name " Timing"].total / (double) (*NanoTrace::Global().timings())[#name " Timing"].count); \
    FailFastAssert( (name##Average) <= maxTime);

#define TraceString(string, traceType, traceDetail) \
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail); }

#define TraceString1(string, traceType, traceDetail, value) \
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail, value); }

#define TraceString2(string, traceType, traceDetail, value1, value2)\
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail, value1, value2); }

#define TraceString3(string, traceType, traceDetail, value1, value2, value3) \
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail, value1, value2, value3); }

#define TraceString4(string, traceType, traceDetail, value1, value2, value3, value4) \
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail, value1, value2, value3, value4); }

#define TraceString5(string, traceType, traceDetail, value1, value2, value3, value4, value5) \
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail, value1, value2, value3, value4, value5); }

#define TraceString6(string, traceType, traceDetail, value1, value2, value3, value4, value5, value6) \
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail, value1, value2, value3, value4, value5, value6); }

#define TraceString7(string, traceType, traceDetail, value1, value2, value3, value4, value5, value6, value7) \
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail, value1, value2, value3, value4, value5, value6, value7); }

#define TraceString8(string, traceType, traceDetail, value1, value2, value3, value4, value5, value6, value7, value8) \
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail, value1, value2, value3, value4, value5, value6, value7, value8); }

#define TraceString9(string, traceType, traceDetail, value1, value2, value3, value4, value5, value6, value7, value8, value9) \
    if(((int) traceType & NanoTrace::Global().allowedTraceType()) && (traceDetail <= NanoTrace::Global().detailLevel())) \
    { NanoTrace::Global().Trace(string, (int) traceType, traceDetail, value1, value2, value3, value4, value5, value6, value7, value8, value9); }

#define TraceStringIf(string, conditional, traceType, traceDetail) \
    if(conditional) { TraceString(string, (int) traceType, traceDetail); }
#define TraceString1If(string, conditional, traceType, traceDetail, value) \
    if(conditional) { TraceString1(string, (int) traceType, traceDetail, value); }
#define TraceString2If(string, conditional, traceType, traceDetail, value1, value2) \
    if(conditional) { TraceString2(string, (int) traceType, traceDetail, value1, value2); }
#define TraceString3If(string, conditional, traceType, traceDetail, value1, value2, value3) \
    if(conditional) { TraceString3(string, (int) traceType, traceDetail, value1, value2, value3); }
#define TraceString4If(string, conditional, traceType, traceDetail, value1, value2, value3, value4) \
    if(conditional) { TraceString4(string, (int) traceType, traceDetail, value1, value2, value3, value4); }

class NanoTrace
{
public:
    NanoTrace(int size);
    
    typedef map<string, NanoTraceTiming> TimingMap;
	void ClearTraces();
    void Close();
    static void ConsolidateFragmentFiles(const string &pathAndFile);
    void CopyLogToFile(unsigned long traceType, const string &pathAndFile);
    void CopyLogToStream(const string &key, unsigned long traceType, shared_ptr<ostream>);
    shared_ptr<ostream> GetLogCopy(const string &key);
    static void FormatTrace(TraceRecord &record, ostream &stream);
	double GetCurrentTime() { return m_stopWatch.getCurrentTime(); }
	shared_ptr<NanoTrace::TimingMap> DetachTimings();
	static string GetTimingString(const string &traceKey, const NanoTraceTiming &timing);
	string GetTimingString(const string &traceKey);
    void LogToFile(const string &filePath, uint32_t maxFileCount = 2);
	string PerformanceTraces();
    void ResetTimingItem(const string &traceKey);
	void ResetTraces();
	// Used to initialize any parameters of the timing if necessary
	void SetTimingParameters(const string &traceKey, NanoTraceTiming &parameters);
    void StopCopyLogToFile();
	string Traces();

    unsigned long allowedTraceType() { return m_allowedTraceType; }
    void allowedTraceType(unsigned long value) { m_allowedTraceType = value; }
    unsigned long debugTraceType() { return m_debugTraceType; }
    void debugTraceType(unsigned long value) { m_allowedTraceType = value; m_debugTraceType = value; }
    TraceDetail detailLevel() { return m_detailLevel; }
    void detailLevel(TraceDetail value) { m_detailLevel = value; }
    static NanoTrace &Global() { return m_nanoTrace; };
    shared_ptr<NanoTrace::TimingMap> &timings() { return m_timings; }

    double TraceTimingOnly(const string &traceKey, const int traceType, const TraceDetail levelOfDetail)
	{
		return TraceImpl(traceKey, -1, traceType, levelOfDetail, true);
	};

    double PerfTraceTimingOnly(const string &traceKey, const double startTime, const int traceType, const TraceDetail levelOfDetail)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, true);
	};

    double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail)
	{
		return TraceImpl(traceKey, -1, traceType, levelOfDetail);
	};

    double PerfTrace(const string &traceKey, const double startTime, const int traceType, const TraceDetail levelOfDetail)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail);
	};

	template <class T> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T value1)
	{
		return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr, 1, lexical_cast<string>(value1).c_str());
	};

	template <class T> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T value1, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 1, lexical_cast<string>(value1).c_str());
	};

    template <class T1, class T2> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2)
	{
        return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr,
            2, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str());

	};

    template <class T1, class T2> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 
            2, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str());
	};

	template <class T1, class T2, class T3> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3)
	{
        return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr,
            3, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str());

	};

    template <class T1, class T2, class T3> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 
            3, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str());
	};

	template <class T1, class T2, class T3, class T4> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4)
	{
        return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr,
            4, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str());

	};

    template <class T1, class T2, class T3, class T4> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 
            4, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str());
	};

	template <class T1, class T2, class T3, class T4, class T5> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5)
	{
        return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr,
            5, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str());

	};

    template <class T1, class T2, class T3, class T4, class T5> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 
            5, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str());

	};

	template <class T1, class T2, class T3, class T4, class T5, class T6> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6)
	{
        return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr,
            6, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str());
	};

    template <class T1, class T2, class T3, class T4, class T5, class T6> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 
            6, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str());
	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6, const T7 &value7)
	{
        return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr,
            7, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str(), lexical_cast<string>(value7).c_str());

	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6, const T7 &value7, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 
            7, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str(), lexical_cast<string>(value7).c_str());
	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6, const T7 &value7, const T8 &value8)
	{
        return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr,
            8, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str(), lexical_cast<string>(value7).c_str(), lexical_cast<string>(value8).c_str());

	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6, const T7 &value7, const T8 &value8, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 
            8, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str(), lexical_cast<string>(value7).c_str(), lexical_cast<string>(value8).c_str());
	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6, const T7 &value7, const T8 &value8, const T9 &value9)
	{
        return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr,
            9, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str(), lexical_cast<string>(value7).c_str(), lexical_cast<string>(value8).c_str(), lexical_cast<string>(value9).c_str());
	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6, const T7 &value7, const T8 &value8, const T9 &value9, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 
            9, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str(), lexical_cast<string>(value7).c_str(), lexical_cast<string>(value8).c_str(), lexical_cast<string>(value9).c_str());
	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10> 
	double Trace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6, const T7 &value7, const T8 &value8, const T9 &value9, const T10 &value10)
	{
        return TraceImpl(traceKey, -1, traceType, levelOfDetail, false, nullptr,
            10, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str(), lexical_cast<string>(value7).c_str(), lexical_cast<string>(value8).c_str(), lexical_cast<string>(value9).c_str(), lexical_cast<string>(value10).c_str());
	};

	template <class T1, class T2, class T3, class T4, class T5, class T6, class T7, class T8, class T9, class T10> 
	double PerfTrace(const string &traceKey, const int traceType, const TraceDetail levelOfDetail, const T1 value1, const T2 &value2, const T3 value3, const T4 &value4, const T5 &value5, 
		const T6 &value6, const T7 &value7, const T8 &value8, const T9 &value9, const T10 &value10, const double startTime)
	{
		return PerfTraceImpl(traceKey, startTime, traceType, levelOfDetail, 
            10, lexical_cast<string>(value1).c_str(), lexical_cast<string>(value2).c_str(), lexical_cast<string>(value3).c_str(), lexical_cast<string>(value4).c_str(), lexical_cast<string>(value5).c_str(),
                lexical_cast<string>(value6).c_str(), lexical_cast<string>(value7).c_str(), lexical_cast<string>(value8).c_str(), lexical_cast<string>(value9).c_str(), lexical_cast<string>(value10).c_str());
	};

private:
    void OpenNextFragmentFile();
	// Returns a startTime that can be passed to PerformanceTrace
	double PerfTraceImpl(const string &traceKey, const double startTime, int traceType, const TraceDetail levelOfDetail, bool timingOnly = false,
                            int dataCount = 0,
                            const char *data1 = nullptr, const char *data2 = nullptr, const char *data3 = nullptr, const char *data4= nullptr, const char *data5 = nullptr,  
                            const char *data6 = nullptr, const char *data7 = nullptr, const char *data8 = nullptr, const char *data9 = nullptr, const char *data10 = nullptr);
    double TraceImpl(const string &traceKey, const double startTime, int traceType, const TraceDetail levelOfDetail, bool timingOnly = false, double *timingResult = nullptr,
                        int dataCount = 0,
                        const char *data1 = nullptr, const char *data2 = nullptr, const char *data3 = nullptr, const char *data4= nullptr, const char *data5 = nullptr,  
                        const char *data6 = nullptr, const char *data7 = nullptr, const char *data8 = nullptr, const char *data9 = nullptr, const char *data10 = nullptr);

    unsigned long m_allowedTraceType;
    unsigned long m_debugTraceType;
    TraceDetail m_detailLevel;
    shared_ptr<Directory> m_directory;
    string m_fileExtension;
    uint32_t m_fileIndex;
    shared_ptr<ofstream> m_fileStream;
    string m_fileWithoutExtension;
    double m_lastTime;
    uint32_t m_maxFileCount;
    int m_maxTraceRecords;
	static NanoTrace m_nanoTrace;
    string m_path;
    static const char *m_replaceTokens[];
	StopWatch m_stopWatch;
	shared_ptr<TimingMap> m_timings;
    vector<TraceRecord> m_traceRecords;
    recursive_mutex traceMutex;
    int m_traceRecordsSize;
    int m_traceRecordsWritten;
    
    // key is a name that can be used to find again, unsigned long is the filter for what traces to collect
    // stream is the stream to put into
    typedef map<string, pair<unsigned long, shared_ptr<ostream>>> TraceCollectorsType;
    TraceCollectorsType m_traceCollectors;
};
