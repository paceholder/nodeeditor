file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(example2 ${CPPS} ${HPPS})

target_link_libraries(example2 nodes)
