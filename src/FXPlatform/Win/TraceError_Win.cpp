//
//  TraceError.cpp
//  FXPlatform
//
//  Created by Eric Zinda on 8/18/15.
//  Copyright (c) 2015 Eric Zinda. All rights reserved.
//

#include "TraceError.h"
#include <string>
using namespace std;

void TraceError::CaptureStack(unsigned int max_frames)
{
	// Nothing to do on Windows
}

string TraceError::Stack() const
{
	// Nothing to do on Windows
	return string();
}