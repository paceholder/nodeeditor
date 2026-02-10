file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(custom_painter ${CPPS} ${HPPS})

target_link_libraries(custom_painter QtNodes)
