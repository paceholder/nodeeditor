cmake_minimum_required(VERSION 2.8.9)

project(blocks)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
ADD_DEFINITIONS(-std=c++11)

# Find the QtWidgets library
find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(Qt5Gui)


include_directories(${Qt5Widgets_INCLUDES})
include_directories(${Qt5Gui_INCLUDES})
include_directories(${Qt5Core_INCLUDES})
add_definitions(${Qt5Widgets_DEFINITIONS})
set(CMAKE_CXX_FLAGS "${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")


# Tell CMake to create the helloworld executable
add_executable(blocks
               main.cpp
               FlowItemEntry.cpp
               Connection.cpp
               FlowItem.cpp
)

# Use the Widgets module from Qt 5.
#qt5_use_modules(blocks Widgets)
target_link_libraries(blocks
  ${Qt5Widgets_LIBRARIES}
  ${Qt5Core_LIBRARIES}
  ${QtGui_LIBRARIES}
  )
