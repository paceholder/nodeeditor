file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(styles ${CPPS} ${HPPS})

target_link_libraries(styles QtNodes)
