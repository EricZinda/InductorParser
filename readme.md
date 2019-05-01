

Inductor Parser
===============
The Inductor Parser is a simple-to-use C++ Template-based parser.  It is small and easy to understand, debug and extend.

It is used in production in an iPhone strategy game called [Exospecies](https://www.exospecies.com). Visit the [Exospecies Blog](https://www.exospecies.com/blog) for more details.

Use it and enjoy!

To Build
--------
indparse is designed to be built with [CMake](https://cmake.org) like this:

1. [Install CMake on your machine](https://cmake.org/install/)
2. Go to the root of the indparse repository and create a build directory. 
	unix: `mkdir build`
	win: `md build`
3. Change to that directory.
	unix and win: `cd build`
4. CMake can build different types of projects using "generators".  Run `cmake -help` to get a list of generators on your system:
	unix and win: `cmake -help`
4. Pick the generator that will create the type of project you want and use the `-G` option to choose it. Here are the ones that have been tested:
	mac make file: 			`cmake -G "Unix Makefiles" ../src`
	Windows Visual Studio: 	`cmake ../src`
5. Then actually do the build using this command which magically builds whatever you choose on the command line: 
	`cmake --build ./`

5a. OR you can manually use the build system that got created by cmake:
	unix or mac make file: 	`make`
	Mac Xcode:				Open the IndProlog.xcodeproj file in the build directory. Build the project using Xcode.
	Windows Visual Studio: 	Open the .sln file in the build directory.  Build the solution from VS.


Directory Structure
-------------------
99.99% of the code for the parser is platform agnostic (or at least should be). It has been built and tested on Windows, Mac and iOS. The platform specific code is located in the iOS and Win directorys and is currently only a single function for debug logging.

- /FXPlatform: 		Contains some general purpose code for tracing, asserts, strings, etc
- /FXPlatform/Parser: The Inductor Parser code
- /FXPlatform/iOS: 	Code specific to iOS and Mac
- /FXPlatform/Win: 	Code specific to Windows
- /Languages:			Example parsers for XML, HTML, CSS. These are the production grammars used in Exospecies. They are simple, take shortcuts, etc.  Treat them as *examples*.

Getting Started
-------------------
Read GettingStarted.md.

License
---------
Do what you like, with no warranties! Read License.md.