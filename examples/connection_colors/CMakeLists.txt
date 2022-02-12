file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(connection_colors ${CPPS} ${HPPS})

target_link_libraries(connection_colors QtNodes)
