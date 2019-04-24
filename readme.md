Inductor Parser
===============

To Build
--------
indparse is designed to be built with [CMake](https://cmake.org) like this:

1. [Install CMake on your machine](https://cmake.org/install/)
2. Go to the root of the indparse repository and create a build directory. 
	unix: `mkdir build`
	win: `md build`
3. Change to that directory.
	unix and win: `cd build`
4. Use CMake to setup the build system:
	unix and win: `cmake ../src`
5. Then actually do the build:
	unix: `make`
	win: Install Visual Studio (free!) and Open the .sln file in the build directory.  Build the solution from VS.