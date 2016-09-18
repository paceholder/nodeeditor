file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(example1 ${CPPS})

target_link_libraries(example1 nodes)
