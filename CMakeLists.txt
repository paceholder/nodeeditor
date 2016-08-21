cmake_minimum_required(VERSION 3.4)

project(NodeEditor)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
ADD_DEFINITIONS(-std=c++11)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core
             Widgets
             Gui
             OpenGL)

add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#############################################################

file(GLOB_RECURSE LIB_CPPS  ./src/*.cpp )

# Tell CMake to create the helloworld executable
add_library(nodes SHARED ${LIB_CPPS})

target_include_directories(nodes INTERFACE "include")

target_link_libraries(nodes
                      Qt5::Core
                      Qt5::Widgets
                      Qt5::Gui
                      Qt5::OpenGL)

#############################################################

file(GLOB_RECURSE EXAMPLE1_CPPS  ./examples/example1/*.cpp )


add_executable(example1 ${EXAMPLE1_CPPS})

target_link_libraries(example1 nodes)

#############################################################

file(GLOB_RECURSE EXAMPLE2_CPPS  ./examples/example2/*.cpp )


add_executable(example2 ${EXAMPLE2_CPPS})

target_link_libraries(example2 nodes)

#############################################################

file(GLOB_RECURSE CALCULATOR_CPPS  ./examples/calculator/*.cpp )


add_executable(calculator ${CALCULATOR_CPPS})

target_link_libraries(calculator nodes)
