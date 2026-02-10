file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(node_validation ${CPPS} ${HPPS})

target_link_libraries(node_validation QtNodes)
