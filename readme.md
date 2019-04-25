

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
4. Use CMake to setup the build system:
	unix and win: `cmake ../src`
5. Then actually do the build:
	unix: `make`
	win: Install Visual Studio (free!) and Open the .sln file in the build directory.  Build the solution from VS.

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