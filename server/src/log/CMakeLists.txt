aux_source_directory(. logsrc)

# set(makelogtest false)
# if(makelogtest)
# 	project(asynclogtest)
# 	# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

# 	find_package(Threads REQUIRED)

# 	set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})

	
# 	add_definitions(-DLOGTEST)
# 	add_executable(${PROJECT_NAME}	
# 		${TESTSRC}
# 	)
# endif(makelogtest)

add_library(log STATIC ${logsrc})

target_link_libraries(log util)