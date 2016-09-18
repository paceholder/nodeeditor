file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(images ${CPPS})

target_link_libraries(images nodes)
