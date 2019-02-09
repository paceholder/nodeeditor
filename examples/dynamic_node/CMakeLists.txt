file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(dynamic_node ${CPPS})

target_link_libraries(dynamic_node nodes)
