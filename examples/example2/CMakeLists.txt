file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(example2 ${CPPS})

target_link_libraries(example2 nodes)
