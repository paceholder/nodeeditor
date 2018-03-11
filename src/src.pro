#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$absolute_path($$PWD/..)

QT += gui widgets opengl
TEMPLATE = lib
TARGET = $$qtLibraryTarget(nodeeditor)
DESTDIR = $$this_dir/lib
DEFINES += \
    NODE_EDITOR_SHARED \
    NODE_EDITOR_EXPORTS

##################################################
# qmake internal options
##################################################

CONFIG += dll warn_off lib_bundle
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
    Compiler.hpp \
    Connection.hpp \
    ConnectionBlurEffect.hpp \
    ConnectionGeometry.hpp \
    ConnectionGraphicsObject.hpp \
    ConnectionPainter.hpp \
    ConnectionState.hpp \
    ConnectionStyle.hpp \
    DataModelRegistry.hpp \
    Export.hpp \
    FlowScene.hpp \
    FlowView.hpp \
    FlowViewStyle.hpp \
    make_unique.hpp \
    Node.hpp \
    NodeConnectionInteraction.hpp \
    NodeData.hpp \
    NodeDataModel.hpp \
    NodeGeometry.hpp \
    NodeGraphicsObject.hpp \
    NodePainter.hpp \
    NodePainterDelegate.hpp \
    NodeState.hpp \
    NodeStyle.hpp \
    OperatingSystem.hpp \
    PortType.hpp \
    Properties.hpp \
    QStringStdHash.hpp \
    QUuidStdHash.hpp \
    Serializable.hpp \
    Style.hpp \
    StyleCollection.hpp

SOURCES += \
    Connection.cpp \
    ConnectionBlurEffect.cpp \
    ConnectionGeometry.cpp \
    ConnectionGraphicsObject.cpp \
    ConnectionPainter.cpp \
    ConnectionState.cpp \
    ConnectionStyle.cpp \
    DataModelRegistry.cpp \
    FlowScene.cpp \
    FlowView.cpp \
    FlowViewStyle.cpp \
    Node.cpp \
    NodeConnectionInteraction.cpp \
    NodeDataModel.cpp \
    NodeGeometry.cpp \
    NodeGraphicsObject.cpp \
    NodePainter.cpp \
    NodeState.cpp \
    NodeStyle.cpp \
    Properties.cpp \
    StyleCollection.cpp

RESOURCES += \
    $$PWD/../resources/resources.qrc

###############################################################
# import libraries
###############################################################

###############################################################
# global commands
###############################################################

win32|unix: {
    commands += echo --- console - $$TARGET --- &

    excludefile = $$PWD/copy.ignore
    !exists("$$excludefile"):excludefile = "$$this_dir/copy.ignore"
    !exists("$$excludefile"):error("$$excludefile" is not exists!)
    win32:excludefile = $$replace(excludefile, /, \\)

    ## copy files
#    dstdir = $$this_dir/lib/nodeeditor/src/
#    win32:dstdir = $$replace(dstdir, /, \\)
#    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
#    win32:srcdir = $$PWD/*.hpp
#    unix:srcdir = $$PWD/
#    win32:srcdir = $$replace(srcdir, /, \\)
#    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
#    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.hpp" &

    ## copy files
#    dstdir = $$this_dir/lib/nodeeditor/include/nodes
#    win32:dstdir = $$replace(dstdir, /, \\)
#    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
#    win32:srcdir = $$PWD/../include/nodes/*.*
#    unix:srcdir = $$PWD/../include/nodes/
#    win32:srcdir = $$replace(srcdir, /, \\)
#    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
#    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.*" &

    QMAKE_POST_LINK += $$commands
}
