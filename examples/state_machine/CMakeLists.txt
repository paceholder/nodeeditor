file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(state_machine ${CPPS})

target_link_libraries(state_machine nodes)
