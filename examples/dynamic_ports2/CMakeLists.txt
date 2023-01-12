file(GLOB_RECURSE CPPS ./*.cpp)
file(GLOB_RECURSE HPPS ./*.hpp)

add_executable(dynamic_ports2 ${CPPS} ${HPPS})

target_link_libraries(dynamic_ports2 QtNodes)
