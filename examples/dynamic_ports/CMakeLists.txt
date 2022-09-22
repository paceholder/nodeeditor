file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(dynamic_ports ${CPPS})

target_link_libraries(dynamic_ports QtNodes)
