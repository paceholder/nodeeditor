cmake_minimum_required(VERSION 3.16)
project(QtPropertyBrowser)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt components
find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

# Qt specific settings
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Check if config.cmake exists and read SOLUTIONS_LIBRARY setting
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake")
    include(config.cmake)
endif()


option(QTPROPERTYBROWSER_USELIB "Use QtPropertyBrowser as a library" ON)

# Set library name and directory
set(QTPROPERTYBROWSER_LIBNAME "Qt6PropertyBrowser")
set(QTPROPERTYBROWSER_LIBDIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Add RPATH for Unix systems when using the library
if(UNIX AND QTPROPERTYBROWSER_USELIB AND NOT QTPROPERTYBROWSER_BUILDLIB)
    set(CMAKE_INSTALL_RPATH "${QTPROPERTYBROWSER_LIBDIR}")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
endif()

# Add subdirectories in order
if(QTPROPERTYBROWSER_USELIB)
    add_subdirectory(buildlib)
endif()
