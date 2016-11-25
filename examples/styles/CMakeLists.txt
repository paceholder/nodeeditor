file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(styles ${CPPS})

target_link_libraries(styles nodes)
