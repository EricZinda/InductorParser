target_sources(parser
    PRIVATE
    	Directory.h
    	FailFast.h
    	FailFast.cpp
    	FileStream.h
        Logger.h
    	NanoTrace.h
    	NanoTrace.cpp
    	ReflectionEnum.h
        Stopwatch.h
        StringHelpers.h
    	TraceError.h
    	Utilities.h
    	Utilities.cpp
)
target_include_directories(parser PRIVATE ./)