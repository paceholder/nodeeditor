file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(connection_colors ${CPPS})

target_link_libraries(connection_colors nodes)
