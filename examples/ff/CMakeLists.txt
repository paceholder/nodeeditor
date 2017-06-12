file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(ff ${CPPS})

target_link_libraries(ff nodes)
