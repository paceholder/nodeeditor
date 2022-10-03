file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(vertical_layout ${CPPS})

target_link_libraries(vertical_layout QtNodes)
