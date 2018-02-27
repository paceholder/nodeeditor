cmake_minimum_required(VERSION 3.2)
# version 3.4 is required as other do not work with C++14 and clang

project(NodeEditor CXX)

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

option(BUILD_EXAMPLES "Build Examples" ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core
             Widgets
             Gui
             OpenGL)

qt5_add_resources(RESOURCES ./resources/resources.qrc)

# If we want to give the option to build a static library,
# leave off SHARED here and use BUILD_SHARED_LIBS to
# choose whether to add -D NODE_EDITOR_SHARED
add_library(nodes SHARED
  src/Connection.cpp
  src/ConnectionBlurEffect.cpp
  src/ConnectionGeometry.cpp
  src/ConnectionGraphicsObject.cpp
  src/ConnectionPainter.cpp
  src/ConnectionState.cpp
  src/ConnectionStyle.cpp
  src/DataModelRegistry.cpp
  src/FlowScene.cpp
  src/FlowView.cpp
  src/FlowViewStyle.cpp
  src/Node.cpp
  src/NodeConnectionInteraction.cpp
  src/NodeDataModel.cpp
  src/NodeGeometry.cpp
  src/NodeGraphicsObject.cpp
  src/NodePainter.cpp
  src/NodeState.cpp
  src/NodeStyle.cpp
  src/Properties.cpp
  src/StyleCollection.cpp
  ${RESOURCES}
)

target_include_directories(nodes
  PUBLIC 
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE src
)

target_link_libraries(nodes
  PUBLIC
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::OpenGL
)

target_compile_definitions(nodes
  PUBLIC
    ${Qt5Widgets_DEFINITIONS}
    NODE_EDITOR_SHARED
  PRIVATE
    NODE_EDITOR_EXPORTS
)

target_compile_options(nodes
  PUBLIC ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /EHsc>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
)

target_compile_features(nodes
  PUBLIC
    cxx_generic_lambdas # Require C++14
)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
