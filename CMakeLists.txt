cmake_minimum_required(VERSION 3.8)

cmake_policy(SET CMP0072 NEW)

project(QtNodes CXX)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(OpenGL_GL_PREFERENCE LEGACY)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

get_directory_property(_has_parent PARENT_DIRECTORY)
if(_has_parent)
  set(is_root_project OFF)
else()
  set(is_root_project ON)
endif()

set(NODES_DEVELOPER_DEFAULTS "${is_root_project}" CACHE BOOL "Turns on default settings for development of QtNodes")

option(BUILD_TESTING "Build tests" "${NODES_DEVELOPER_DEFAULTS}")
option(BUILD_EXAMPLES "Build Examples" "${NODES_DEVELOPER_DEFAULTS}")
option(BUILD_DOCS "Build Documentation" "${NODES_DEVELOPER_DEFAULTS}")
option(BUILD_SHARED_LIBS "Build as shared library" ON)
option(BUILD_DEBUG_POSTFIX_D "Append d suffix to debug libraries" OFF)
option(NODES_FORCE_TEST_COLOR "Force colorized unit test output" OFF)

enable_testing()

if(NODES_DEVELOPER_DEFAULTS)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()

if(BUILD_DEBUG_POSTFIX_D)
  set(CMAKE_DEBUG_POSTFIX d)
endif()

add_subdirectory(external)


# Find the QtWidgets library
find_package(Qt6
  COMPONENTS
   Core
   Widgets
   Gui
   OpenGL
)

if (NOT Qt6_FOUND)
  find_package(Qt5 5.13
    COMPONENTS
     Core
     Widgets
     Gui
     OpenGL
  )
endif()

if (NOT (Qt6_FOUND OR Qt5_FOUND))
  message(FATAL_ERRROR "Qt libraries were not found.")
endif()

qt_add_resources(RESOURCES ./resources/resources.qrc)

# Unfortunately, as we have a split include/src, AUTOMOC doesn't work.
# We'll have to manually specify some files
set(CMAKE_AUTOMOC ON)

set(CPP_SOURCE_FILES
  src/AbstractGraphModel.cpp
  src/BasicGraphicsScene.cpp
  src/ConnectionGraphicsObject.cpp
  src/ConnectionPainter.cpp
  src/ConnectionState.cpp
  src/ConnectionStyle.cpp
  src/DataFlowGraphModel.cpp
  src/DataFlowGraphicsScene.cpp
  src/DataModelRegistry.cpp
  src/GraphicsView.cpp
  src/GraphicsViewStyle.cpp
  src/NodeConnectionInteraction.cpp
  src/NodeDataModel.cpp
  src/NodeGeometry.cpp
  src/NodeGraphicsObject.cpp
  src/NodePainter.cpp
  src/NodeState.cpp
  src/NodeStyle.cpp
  src/StyleCollection.cpp
  src/locateNode.cpp
)

# If we want to give the option to build a static library,
# set BUILD_SHARED_LIBS option to OFF
add_library(nodes ${CPP_SOURCE_FILES} ${RESOURCES})

add_library(QtNodes::nodes ALIAS nodes)


target_include_directories(nodes
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/nodes/internal>
)

target_link_libraries(nodes
  PUBLIC
    Qt::Core
    Qt::Widgets
    Qt::Gui
    Qt::OpenGL
)

target_compile_definitions(nodes
  PUBLIC
    NODE_EDITOR_SHARED
  PRIVATE
    NODE_EDITOR_EXPORTS
    #NODE_DEBUG_DRAWING
    QT_NO_KEYWORDS
)

target_compile_options(nodes
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4127 /EHsc>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
)
if(NOT "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
  # Clang-Cl on MSVC identifies as "Clang" but behaves more like MSVC:
  target_compile_options(nodes
    PRIVATE
      $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
  )
endif()

if(NODES_DEVELOPER_DEFAULTS)
  target_compile_features(nodes PUBLIC cxx_std_14)
  set_target_properties(nodes PROPERTIES CXX_EXTENSIONS OFF)
endif()

set_target_properties(nodes
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

######
# Moc
##

file(GLOB_RECURSE HEADERS_TO_MOC include/nodes/internal/*.hpp)

qt_wrap_cpp(nodes_moc
    ${HEADERS_TO_MOC}
  TARGET nodes
  OPTIONS --no-notes # Don't display a note for the headers which don't produce a moc_*.cpp
)

target_sources(nodes PRIVATE ${nodes_moc})

###########
# Examples
##

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()

if(BUILD_DOCS)
  add_subdirectory(docs)
endif()

##################
# Automated Tests
##

if(BUILD_TESTING)
  #add_subdirectory(test)
endif()

###############
# Installation
##

include(GNUInstallDirs)

set(INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/QtNodes)

install(TARGETS nodes
  EXPORT QtNodesTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT QtNodesTargets
  FILE QtNodesTargets.cmake
  NAMESPACE QtNodes::
  DESTINATION ${INSTALL_CONFIGDIR}
)

include(CMakePackageConfigHelpers)

configure_package_config_file(${CMAKE_CURRENT_LIST_DIR}/cmake/QtNodesConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/QtNodesConfig.cmake
  INSTALL_DESTINATION ${INSTALL_CONFIGDIR}
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/QtNodesConfig.cmake
  DESTINATION ${INSTALL_CONFIGDIR}
)
