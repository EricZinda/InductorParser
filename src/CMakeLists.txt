cmake_minimum_required(VERSION 3.14)

project(IndParser CXX)
set(CMAKE_CXX_STANDARD 11)

add_library(parser "")
target_include_directories(parser PRIVATE ./)

add_executable (gparse main.cpp)
target_include_directories(gparse PRIVATE ./)

add_subdirectory(FXPlatform)
if(WIN32)
	add_subdirectory(FXPlatform/Win)
else()
	target_link_libraries(gparse parser "-framework Foundation")
	add_subdirectory(FXPlatform/iOS)
endif()

add_subdirectory(FXPlatform/Parser)
add_subdirectory(Languages)


