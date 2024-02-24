cmake_minimum_required(VERSION 3.27.4)

project(tinymuduo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

# add_definitions(-DNDEBUG)
# add_definitions(-Wall)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(./include)
include_directories(./include/log)
include_directories(./include/thread)
include_directories(./include/util)

add_subdirectory(./src)
