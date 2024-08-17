set(CPPS main.cpp)

add_executable(plugins_load ${CPPS})

target_link_libraries(plugins_load QtNodes)

add_subdirectory(plugins/plugin_text)
