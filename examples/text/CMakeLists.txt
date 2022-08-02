file(GLOB_RECURSE CPPS  ./*.cpp )

add_executable(text ${CPPS})

target_link_libraries(text nodes)
