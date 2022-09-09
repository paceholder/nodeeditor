file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(text ${CPPS} ${HPPS})

target_link_libraries(text QtNodes)
