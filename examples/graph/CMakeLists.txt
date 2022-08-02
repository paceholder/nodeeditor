file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(graph ${CPPS})

target_link_libraries(graph nodes)
