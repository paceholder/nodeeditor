# common.cmake - Common settings for QtPropertyBrowser

# Check if config.cmake exists and read SOLUTIONS_LIBRARY setting
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/config.cmake")
    include(config.cmake)
    if(DEFINED SOLUTIONS_LIBRARY AND SOLUTIONS_LIBRARY STREQUAL "yes")
        set(QTPROPERTYBROWSER_USELIB ON CACHE BOOL "Use QtPropertyBrowser as a library" FORCE)
    endif()
endif()

# Set library name (equivalent to qtLibraryTarget)
# In Qt6, we typically just use the library name directly
set(QTPROPERTYBROWSER_LIBNAME "Qt6PropertyBrowser")

# Set library directory
set(QTPROPERTYBROWSER_LIBDIR "${CMAKE_CURRENT_SOURCE_DIR}/lib")

# Add RPATH for Unix systems when using the library but not building it
if(UNIX AND QTPROPERTYBROWSER_USELIB AND NOT QTPROPERTYBROWSER_BUILDLIB)
    list(APPEND CMAKE_INSTALL_RPATH "${QTPROPERTYBROWSER_LIBDIR}")
    set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
    set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# Export these variables for use in subdirectories
set(QTPROPERTYBROWSER_LIBNAME ${QTPROPERTYBROWSER_LIBNAME} PARENT_SCOPE)
set(QTPROPERTYBROWSER_LIBDIR ${QTPROPERTYBROWSER_LIBDIR} PARENT_SCOPE)