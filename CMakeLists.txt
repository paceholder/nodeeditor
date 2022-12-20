cmake_minimum_required(VERSION 3.8)

cmake_policy(SET CMP0072 NEW) # new in 3.11. The NEW behavior for this policy is to set OpenGL_GL_PREFERENCE to GLVND.
cmake_policy(SET CMP0068 NEW) # new in 3.9. The NEW behavior of this policy is to ignore the RPATH settings for install_name on macOS.


project(QtNodesLibrary CXX)

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

set(QT_NODES_DEVELOPER_DEFAULTS "${is_root_project}" CACHE BOOL "Turns on default settings for development of QtNodes")

option(BUILD_TESTING "Build tests" "${QT_NODES_DEVELOPER_DEFAULTS}")
option(BUILD_EXAMPLES "Build Examples" "${QT_NODES_DEVELOPER_DEFAULTS}")
option(BUILD_DOCS "Build Documentation" "${QT_NODES_DEVELOPER_DEFAULTS}")
option(BUILD_SHARED_LIBS "Build as shared library" ON)
option(BUILD_DEBUG_POSTFIX_D "Append d suffix to debug libraries" OFF)
option(QT_NODES_FORCE_TEST_COLOR "Force colorized unit test output" OFF)

enable_testing()

if(QT_NODES_DEVELOPER_DEFAULTS)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
  set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
  set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif()

if(BUILD_DEBUG_POSTFIX_D)
  set(CMAKE_DEBUG_POSTFIX "d")
  set(CMAKE_RELEASE_POSTFIX "")
  set(CMAKE_RELWITHDEBINFO_POSTFIX "rd")
  set(CMAKE_MINSIZEREL_POSTFIX "s")
endif()

add_subdirectory(external)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Gui OpenGL)
message(STATUS "QT_VERSION: ${QT_VERSION}, QT_DIR: ${QT_DIR}")

if (${QT_VERSION} VERSION_LESS 5.11.0)
  message(FATAL_ERROR "Requires qt version >= 5.11.0, Your current version is ${QT_VERSION}")
endif()

if (${QT_VERSION_MAJOR} EQUAL 6)
  qt_add_resources(RESOURCES ./resources/resources.qrc)
else()
  qt5_add_resources(RESOURCES ./resources/resources.qrc)
endif()

# Unfortunately, as we have a split include/src, AUTOMOC doesn't work.
# We'll have to manually specify some files
set(CMAKE_AUTOMOC ON)

set(CPP_SOURCE_FILES
  src/AbstractGraphModel.cpp
  src/AbstractNodeGeometry.cpp
  src/BasicGraphicsScene.cpp
  src/ConnectionGraphicsObject.cpp
  src/ConnectionPainter.cpp
  src/ConnectionState.cpp
  src/ConnectionStyle.cpp
  src/DataFlowGraphModel.cpp
  src/DataFlowGraphicsScene.cpp
  src/DefaultHorizontalNodeGeometry.cpp
  src/DefaultVerticalNodeGeometry.cpp
  src/Definitions.cpp
  src/GraphicsView.cpp
  src/GraphicsViewStyle.cpp
  src/NodeDelegateModelRegistry.cpp
  src/NodeConnectionInteraction.cpp
  src/NodeDelegateModel.cpp
  src/NodeGraphicsObject.cpp
  src/DefaultNodePainter.cpp
  src/NodeState.cpp
  src/NodeStyle.cpp
  src/StyleCollection.cpp
  src/UndoCommands.cpp
  src/locateNode.cpp
)

set(HPP_HEADER_FILES
  include/QtNodes/internal/AbstractGraphModel.hpp
  include/QtNodes/internal/AbstractNodeGeometry.hpp
  include/QtNodes/internal/AbstractNodePainter.hpp
  include/QtNodes/internal/BasicGraphicsScene.hpp
  include/QtNodes/internal/Compiler.hpp
  include/QtNodes/internal/ConnectionGraphicsObject.hpp
  include/QtNodes/internal/ConnectionIdHash.hpp
  include/QtNodes/internal/ConnectionIdUtils.hpp
  include/QtNodes/internal/ConnectionState.hpp
  include/QtNodes/internal/ConnectionStyle.hpp
  include/QtNodes/internal/DataFlowGraphicsScene.hpp
  include/QtNodes/internal/DataFlowGraphModel.hpp
  include/QtNodes/internal/DefaultNodePainter.hpp
  include/QtNodes/internal/Definitions.hpp
  include/QtNodes/internal/Export.hpp
  include/QtNodes/internal/GraphicsView.hpp
  include/QtNodes/internal/GraphicsViewStyle.hpp
  include/QtNodes/internal/locateNode.hpp
  include/QtNodes/internal/NodeData.hpp
  include/QtNodes/internal/NodeDelegateModel.hpp
  include/QtNodes/internal/NodeDelegateModelRegistry.hpp
  include/QtNodes/internal/NodeGraphicsObject.hpp
  include/QtNodes/internal/NodeState.hpp
  include/QtNodes/internal/NodeStyle.hpp
  include/QtNodes/internal/OperatingSystem.hpp
  include/QtNodes/internal/QStringStdHash.hpp
  include/QtNodes/internal/QUuidStdHash.hpp
  include/QtNodes/internal/Serializable.hpp
  include/QtNodes/internal/Style.hpp
  include/QtNodes/internal/StyleCollection.hpp
  src/ConnectionPainter.hpp
  src/DefaultHorizontalNodeGeometry.hpp
  src/DefaultVerticalNodeGeometry.hpp
  src/NodeConnectionInteraction.hpp
  src/UndoCommands.hpp
)

# If we want to give the option to build a static library,
# set BUILD_SHARED_LIBS option to OFF
add_library(QtNodes
  ${CPP_SOURCE_FILES}
  ${HPP_HEADER_FILES}
  ${RESOURCES}
)

add_library(QtNodes::QtNodes ALIAS QtNodes)


target_include_directories(QtNodes
  PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/QtNodes/internal>
)

target_link_libraries(QtNodes
  PUBLIC
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::OpenGL
)

target_compile_definitions(QtNodes
  PUBLIC
    NODE_EDITOR_SHARED
  PRIVATE
    NODE_EDITOR_EXPORTS
    #NODE_DEBUG_DRAWING
    QT_NO_KEYWORDS
)


target_compile_options(QtNodes
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /wd4127 /EHsc /utf-8>
    $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra>
    $<$<CXX_COMPILER_ID:AppleClang>:-Wall -Wextra -Werror>
)
if(NOT "${CMAKE_CXX_SIMULATE_ID}" STREQUAL "MSVC")
  # Clang-Cl on MSVC identifies as "Clang" but behaves more like MSVC:
  target_compile_options(QtNodes
    PRIVATE
      $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra>
  )
endif()

if(QT_NODES_DEVELOPER_DEFAULTS)
  target_compile_features(QtNodes PUBLIC cxx_std_14)
  set_target_properties(QtNodes PROPERTIES CXX_EXTENSIONS OFF)
endif()


set_target_properties(QtNodes
  PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

######
# Moc
##

file(GLOB_RECURSE HEADERS_TO_MOC include/QtNodes/internal/*.hpp)

if (${QT_VERSION_MAJOR} EQUAL 6)
  qt_wrap_cpp(nodes_moc
      ${HEADERS_TO_MOC}
      TARGET QtNodes
    OPTIONS --no-notes # Don't display a note for the headers which don't produce a moc_*.cpp
  )
else()
  qt5_wrap_cpp(nodes_moc
  ${HEADERS_TO_MOC}
  TARGET QtNodes
  OPTIONS --no-notes # Don't display a note for the headers which don't produce a moc_*.cpp
  )
endif()

target_sources(QtNodes PRIVATE ${nodes_moc})

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

install(TARGETS QtNodes
  EXPORT QtNodesTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
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
