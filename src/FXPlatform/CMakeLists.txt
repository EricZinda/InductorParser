target_sources(parser
    PRIVATE
    	Directory.h
    	FailFast.h
    	FailFast.cpp
    	FileStream.h
    	NanoTrace.h
    	NanoTrace.cpp
    	ReflectionEnum.h
	Stopwatch.h
    	TraceError.h
    	Utilities.h
    	Utilities.cpp
)
target_include_directories(parser PRIVATE ./)