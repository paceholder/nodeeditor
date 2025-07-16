# CMakeLists.txt for building QtPropertyBrowser library

# Set build configuration
set(QTPROPERTYBROWSER_BUILDLIB TRUE)

# Include the qtpropertybrowser settings
include(${CMAKE_CURRENT_SOURCE_DIR}/../src/qtpropertybrowser.cmake)

# Create the shared library
add_library(${QTPROPERTYBROWSER_LIBNAME} SHARED
    ${QTPROPERTYBROWSER_SOURCES}
    ${QTPROPERTYBROWSER_HEADERS}
    ${QTPROPERTYBROWSER_RESOURCES}
)

# Link Qt libraries
target_link_libraries(${QTPROPERTYBROWSER_LIBNAME}
    PUBLIC
        Qt6::Core
        Qt6::Widgets
)

# Set target properties
set_target_properties(${QTPROPERTYBROWSER_LIBNAME} PROPERTIES
    VERSION 1.0.0
    SOVERSION 1
    DEBUG_POSTFIX d  # Add 'd' suffix for debug builds
)

# Set output directories
set_target_properties(${QTPROPERTYBROWSER_LIBNAME} PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${QTPROPERTYBROWSER_LIBDIR}
    RUNTIME_OUTPUT_DIRECTORY ${QTPROPERTYBROWSER_LIBDIR}
    ARCHIVE_OUTPUT_DIRECTORY ${QTPROPERTYBROWSER_LIBDIR}
)

# Platform-specific settings
if(APPLE)
    # Equivalent to CONFIG += absolute_library_soname
    set_target_properties(${QTPROPERTYBROWSER_LIBNAME} PROPERTIES
        MACOSX_RPATH ON
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif()

# Windows-specific settings
if(WIN32)
    # Export definition
    target_compile_definitions(${QTPROPERTYBROWSER_LIBNAME} PRIVATE QT_QTPROPERTYBROWSER_EXPORT)
    
    # Set DLL output directory for Windows
    set_target_properties(${QTPROPERTYBROWSER_LIBNAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY_DEBUG ${QTPROPERTYBROWSER_LIBDIR}
        RUNTIME_OUTPUT_DIRECTORY_RELEASE ${QTPROPERTYBROWSER_LIBDIR}
    )
    
    # Get Qt installation binary directory
    get_target_property(QT_BIN_DIR Qt6::Core LOCATION)
    get_filename_component(QT_BIN_DIR ${QT_BIN_DIR} DIRECTORY)
    get_filename_component(QT_BIN_DIR ${QT_BIN_DIR} DIRECTORY)
    set(QT_BIN_DIR "${QT_BIN_DIR}/bin")
    
    # Install DLL to Qt bin directory (optional, comment out if not needed)
    install(TARGETS ${QTPROPERTYBROWSER_LIBNAME}
        RUNTIME DESTINATION ${QT_BIN_DIR}
    )
endif()

# Include directories
target_include_directories(${QTPROPERTYBROWSER_LIBNAME}
    PUBLIC
        $<BUILD_INTERFACE:${QTPROPERTYBROWSER_DIR}>
        $<INSTALL_INTERFACE:include/qtpropertybrowser>
)

# Multi-configuration generators (Visual Studio, Xcode)
if(CMAKE_CONFIGURATION_TYPES)
    # Set output directories for each configuration
    foreach(CONFIG ${CMAKE_CONFIGURATION_TYPES})
        string(TOUPPER ${CONFIG} CONFIG_UPPER)
        set_target_properties(${QTPROPERTYBROWSER_LIBNAME} PROPERTIES
            LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${QTPROPERTYBROWSER_LIBDIR}
            RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${QTPROPERTYBROWSER_LIBDIR}
            ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} ${QTPROPERTYBROWSER_LIBDIR}
        )
    endforeach()
endif()

# Install rules
install(TARGETS ${QTPROPERTYBROWSER_LIBNAME}
    EXPORT QtPropertyBrowserTargets
    LIBRARY DESTINATION ${QTPROPERTYBROWSER_LIBDIR}
    ARCHIVE DESTINATION ${QTPROPERTYBROWSER_LIBDIR}
    RUNTIME DESTINATION ${QTPROPERTYBROWSER_LIBDIR}
)

# Install headers
install(FILES ${QTPROPERTYBROWSER_HEADERS}
    DESTINATION include/qtpropertybrowser
)

# Export targets for find_package support (optional)
install(EXPORT QtPropertyBrowserTargets
    FILE QtPropertyBrowserTargets.cmake
    NAMESPACE QtPropertyBrowser::
    DESTINATION lib/cmake/QtPropertyBrowser
)