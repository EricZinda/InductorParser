target_sources(parser
    PRIVATE
    	FailFast.h
    	FailFast.cpp
        	Logger.h
    	NanoTrace.h
    	NanoTrace.cpp
    	ReflectionEnum.h
    	Utilities.h
    	Utilities.cpp
)
target_include_directories(parser PRIVATE ./)