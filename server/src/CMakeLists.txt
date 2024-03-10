
aux_source_directory(. MAINSRC)
list(REMOVE_ITEM MAINSRC ./test.cpp)

add_subdirectory(./util)
add_subdirectory(./log)
add_subdirectory(./server)
add_subdirectory(./thread)

add_executable(${PROJECT_NAME} ${MAINSRC})

find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} Threads::Threads)

target_link_libraries(${PROJECT_NAME} util log server thread)
