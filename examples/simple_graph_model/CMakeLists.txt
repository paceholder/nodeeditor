file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(simple_graph_model ${CPPS} ${HPPS})

target_link_libraries(simple_graph_model QtNodes)
