cmake_minimum_required(VERSION 2.8.11)

project(blocks)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
ADD_DEFINITIONS(-std=c++11)


set(QT_PATH "~/projects/geoqt5_thirdparty/Linux/Qt")
set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_PATH})

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

target_link_libraries(example1
                      nodes)
