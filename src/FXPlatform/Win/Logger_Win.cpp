//
//  Logger.cpp
//  GameLib
//
//  Created by Eric Zinda on 5/19/15.
//  Copyright (c) 2015 Eric Zinda. All rights reserved.
//

#include "Logger.h"
#include "Windows.h"

void DebugLogMessage(int traceType, const TraceDetail levelOfDetail, const char *message)
{
	OutputDebugString(message);
}
