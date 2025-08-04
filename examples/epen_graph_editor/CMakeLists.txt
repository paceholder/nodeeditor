file(GLOB_RECURSE CPPS  ./src/*.cpp )
file(GLOB_RECURSE HPPS  ./include/*.hpp )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Always build with all compiler support
# The runtime will check availability
add_definitions(-DALWAYS_INCLUDE_ALL_COMPILERS)

add_executable(epen_graph_editor ${CPPS} ${HPPS})

target_include_directories(epen_graph_editor
  PUBLIC
    include
)

# Base libraries
set(BASE_LIBS QtPropertyBrowser QtNodes Qt6::Core Qt6::Widgets ${QSCINTILLA_TARGET_NAME})

target_link_libraries(epen_graph_editor PRIVATE ${BASE_LIBS})

# Platform-specific settings
if(APPLE)
    # On macOS, we might need to link against system frameworks for dynamic loading
    find_library(DL_LIBRARY dl)
    if(DL_LIBRARY)
        target_link_libraries(epen_graph_editor PRIVATE ${DL_LIBRARY})
    endif()
endif()

if(UNIX AND NOT APPLE)
    # On Linux, we need libdl for dynamic loading
    target_link_libraries(epen_graph_editor PRIVATE dl)
endif()

# Always enable runtime detection for all compilers
add_definitions(-DRUNTIME_GPU_DETECTION)