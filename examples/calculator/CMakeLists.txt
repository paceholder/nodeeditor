file(GLOB_RECURSE CPPS  ./*.cpp )
file(GLOB_RECURSE HPPS  ./*.hpp)

add_executable(calculator ${CPPS} ${HPPS})

target_link_libraries(calculator nodes)
