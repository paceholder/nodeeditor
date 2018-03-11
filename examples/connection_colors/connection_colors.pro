#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$absolute_path($$PWD/../..)

QT += gui widgets
TEMPLATE = app
TARGET = $$qtLibraryTarget(connection_colors)
DESTDIR = $$this_dir/lib
DEFINES += NODE_EDITOR_SHARED

##################################################
# qmake internal options
##################################################

CONFIG += warn_off lib_bundle
#CONFIG += no_kewords slient
#DEFINES += QT_NO_DEBUG_OUTPUT

##################################################
# release/debug mode
##################################################

win32 {
    # On Windows you can't mix release and debug libraries.
    # The designer is built in release mode. If you like to use it
    # you need a release version.For your own application development you
    # might need a debug version.
    # Enable debug_and_release + build_all if you want to build both.

    CONFIG      += debug_and_release
    CONFIG += build_all
#    CONFIG      += QT_NO_DEBUG_OUTPUT

} else {
    CONFIG      += release
}

linux-g++ {
#    CONFIG      += separate_debug_info
}

##################################################
# creating a precompiled header file (only supported
# on some platforms (Windows - all MSVC project types,
# Mac OS X - Xcode, Makefile, Unix - gcc 3.3 and up)
##################################################

win32 {
#    CONFIG += precompile_header
#    PRECOMPILED_HEADER = $$PWD/precomp.h
#    INCLUDEPATH += $$PWD
}

###############################################################
# resource files
###############################################################

HEADERS += \
    models.hpp

SOURCES += \
    main.cpp \
    models.cpp

###############################################################
# import libraries
###############################################################

## import nodeeditor library
win32:CONFIG(release, debug|release):LIBS += -L$$this_dir/lib -lnodeeditor
else:win32:CONFIG(debug, debug|release):LIBS += -L$$this_dir/lib -lnodeeditord
else:unix:LIBS += -L$$this_dir/lib -lnodeeditor
INCLUDEPATH += $$this_dir/include
DEPENDPATH += $$this_dir/include

###############################################################
# global commands
###############################################################

win32|unix: {
    ##
}
