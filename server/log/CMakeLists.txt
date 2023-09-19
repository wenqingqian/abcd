cmake_minimum_required(VERSION 3.27.4)

project(asynclogtest)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

find_package(Threads REQUIRED)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB TESTSRC ./*.cpp)
add_definitions(-DLOGTEST)
add_executable(${PROJECT_NAME}	
	${TESTSRC}
)

include_directories(./include)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

project(asynclog)

list(REMOVE_ITEM TESTSRC ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

add_library(ASYNCLOG STATIC ${TESTSRC})