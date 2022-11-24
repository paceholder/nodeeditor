file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(images ${CPPS} ${HPPS})

target_link_libraries(images nodes)
