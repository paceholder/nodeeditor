file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(simple_graph_model ${CPPS})

target_link_libraries(simple_graph_model QtNodes)
