cmake_minimum_required(VERSION 3.27.4)

project(asynclog)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(Threads REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR})

add_executable(${PROJECT_NAME}	
	main.cpp
	currentThread.cpp
	AsyncLogger.cpp
	LogFile.cpp
	Logger.cpp
	LogStream.cpp
)

include_directories(./include)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)