cmake_minimum_required(VERSION 3.14)

project(IndParser CXX)
set(CMAKE_CXX_STANDARD 11)

add_library(parser "")
target_include_directories(parser PRIVATE ./)

add_executable (indparse main.cpp)
target_include_directories(indparse PRIVATE ./)

add_subdirectory(FXPlatform)
if(WIN32)
	# Link with Kernel32.lib so we can output debug messages
	target_link_libraries(indparse parser Kernel32.lib)
	add_subdirectory(FXPlatform/Win)
else()
	target_link_libraries(indparse parser "-framework Foundation")
	add_subdirectory(FXPlatform/iOS)
endif()

add_subdirectory(FXPlatform/Parser)
add_subdirectory(FXPlatform/Languages)


