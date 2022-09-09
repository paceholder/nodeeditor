file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp )

add_executable(resizable_images ${CPPS} ${HPPS})

target_link_libraries(resizable_images QtNodes)
