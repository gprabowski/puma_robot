cmake_minimum_required(VERSION 3.23)
project(gk2)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

message("ADDING THIRDPARTY LIBRARIES")
add_subdirectory(thirdparty)
message("ADDING TINYCAD EXECUTABLE")
add_subdirectory(src)


