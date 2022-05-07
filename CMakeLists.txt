cmake_minimum_required(VERSION 3.20)
project(gk2)
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")
endif()
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("ADDING THIRDPARTY LIBRARIES")
add_subdirectory(thirdparty)
message("ADDING TINYCAD EXECUTABLE")
add_subdirectory(src)


