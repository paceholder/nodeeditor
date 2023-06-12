if(MSVC)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:plugins_load>/plugins")
else()
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "$<TARGET_FILE_DIR:plugins_load>/plugins")
endif()

file(GLOB_RECURSE CPPS ./*.cpp)
file(GLOB_RECURSE HPPS ./*.hpp)

add_library(plugin_text SHARED ${CPPS} ${HPPS})

target_link_libraries(plugin_text QtNodes)

target_compile_definitions(plugin_text PUBLIC NODE_EDITOR_SHARED)

set_target_properties(plugin_text PROPERTIES SUFFIX ".node")
