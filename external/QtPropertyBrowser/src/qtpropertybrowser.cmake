# qtpropertybrowser.cmake - Include file for QtPropertyBrowser

# Include common settings
include(${CMAKE_CURRENT_LIST_DIR}/../common.cmake)

# Current directory
set(QTPROPERTYBROWSER_DIR ${CMAKE_CURRENT_LIST_DIR})

# Determine if we should use the library or compile sources
if(QTPROPERTYBROWSER_USELIB AND NOT QTPROPERTYBROWSER_BUILDLIB)
    # Use pre-built library
    link_directories(${QTPROPERTYBROWSER_LIBDIR})
    set(QTPROPERTYBROWSER_LIBRARIES ${QTPROPERTYBROWSER_LIBNAME})
else()
    # Compile sources directly
    add_compile_definitions(QT_DISABLE_DEPRECATED_BEFORE=0)
    
    # Define source files
    set(QTPROPERTYBROWSER_SOURCES
        ${QTPROPERTYBROWSER_DIR}/qtpropertybrowser.cpp
        ${QTPROPERTYBROWSER_DIR}/qtpropertymanager.cpp
        ${QTPROPERTYBROWSER_DIR}/qteditorfactory.cpp
        ${QTPROPERTYBROWSER_DIR}/qtvariantproperty.cpp
        ${QTPROPERTYBROWSER_DIR}/qttreepropertybrowser.cpp
        ${QTPROPERTYBROWSER_DIR}/qtbuttonpropertybrowser.cpp
        ${QTPROPERTYBROWSER_DIR}/qtgroupboxpropertybrowser.cpp
        ${QTPROPERTYBROWSER_DIR}/qtpropertybrowserutils.cpp
    )
    
    # Define header files
    set(QTPROPERTYBROWSER_HEADERS
        ${QTPROPERTYBROWSER_DIR}/qtpropertybrowser.h
        ${QTPROPERTYBROWSER_DIR}/qtpropertymanager.h
        ${QTPROPERTYBROWSER_DIR}/qteditorfactory.h
        ${QTPROPERTYBROWSER_DIR}/qtvariantproperty.h
        ${QTPROPERTYBROWSER_DIR}/qttreepropertybrowser.h
        ${QTPROPERTYBROWSER_DIR}/qtbuttonpropertybrowser.h
        ${QTPROPERTYBROWSER_DIR}/qtgroupboxpropertybrowser.h
        ${QTPROPERTYBROWSER_DIR}/qtpropertybrowserutils_p.h
    )
    
    # Define resource files
    set(QTPROPERTYBROWSER_RESOURCES
        ${QTPROPERTYBROWSER_DIR}/qtpropertybrowser.qrc
    )
endif()

# Windows-specific definitions
if(WIN32)
    # Check if we're building a shared library
    get_target_property(target_type ${PROJECT_NAME} TYPE)
    if(target_type STREQUAL "SHARED_LIBRARY")
        add_compile_definitions(QT_QTPROPERTYBROWSER_EXPORT)
    elseif(QTPROPERTYBROWSER_USELIB)
        add_compile_definitions(QT_QTPROPERTYBROWSER_IMPORT)
    endif()
endif()

# Export variables to parent scope
if(NOT QTPROPERTYBROWSER_USELIB OR QTPROPERTYBROWSER_BUILDLIB)
    set(QTPROPERTYBROWSER_SOURCES ${QTPROPERTYBROWSER_SOURCES} PARENT_SCOPE)
    set(QTPROPERTYBROWSER_HEADERS ${QTPROPERTYBROWSER_HEADERS} PARENT_SCOPE)
    set(QTPROPERTYBROWSER_RESOURCES ${QTPROPERTYBROWSER_RESOURCES} PARENT_SCOPE)
endif()
set(QTPROPERTYBROWSER_LIBRARIES ${QTPROPERTYBROWSER_LIBRARIES} PARENT_SCOPE)
set(QTPROPERTYBROWSER_DIR ${QTPROPERTYBROWSER_DIR} PARENT_SCOPE)