file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(lock_nodes_and_connections ${CPPS})

target_link_libraries(lock_nodes_and_connections QtNodes)
