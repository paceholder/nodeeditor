file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(resizable_images ${CPPS})

target_link_libraries(resizable_images QtNodes)
