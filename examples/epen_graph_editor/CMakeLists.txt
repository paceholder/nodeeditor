file(GLOB_RECURSE CPPS  ./src/*.cpp )
file(GLOB_RECURSE HPPS  ./include/*.hpp )

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

add_executable(epen_graph_editor ${CPPS} ${HPPS})

target_include_directories(epen_graph_editor
  PUBLIC
    include
)

target_link_libraries(epen_graph_editor PRIVATE  QtPropertyBrowser QtNodes Qt6::Core
    Qt6::Widgets)
