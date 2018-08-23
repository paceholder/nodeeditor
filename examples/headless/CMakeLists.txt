file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(headless ${CPPS})

target_link_libraries(headless nodes)
