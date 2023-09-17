cmake_minimum_required(VERSION 3.27.4)

project(tinymuduo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

include_directories(./include)
include_directories(./log/include)

aux_source_directory(./ SRC)

list(REMOVE_ITEM SRC ./test.cpp)
add_executable(${PROJECT_NAME} ${SRC})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

add_subdirectory(./log)
target_link_libraries(${PROJECT_NAME} PRIVATE ASYNCLOG)
