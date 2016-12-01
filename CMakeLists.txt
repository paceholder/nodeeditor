cmake_minimum_required(VERSION 3.2)

project(NodeEditor)

option(BUILD_EXAMPLES "ON" ON)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core
             Widgets
             Gui
             OpenGL)

add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

IF (MSVC)
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
ENDIF (MSVC)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#############################################################

file(GLOB_RECURSE LIB_CPPS  ./src/*.cpp )

qt5_add_resources(RESOURCES ./resources/resources.qrc)

# Tell CMake to create the helloworld executable
add_library(nodes SHARED ${LIB_CPPS} ${RESOURCES})

target_include_directories(nodes INTERFACE "include")

target_compile_definitions(nodes PUBLIC "-DNODE_EDITOR_SHARED")
target_compile_definitions(nodes PRIVATE "-DNODE_EDITOR_EXPORTS")

target_link_libraries(nodes
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Gui
                      Qt5::OpenGL)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif()
