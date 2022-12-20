get_filename_component(QtNodes_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include(CMakeFindDependencyMacro)

# NOTE Had to use find_package because find_dependency does not support COMPONENTS or MODULE until 3.8.0

find_dependency(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_dependency(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS
                Core
                Widgets
                Gui
                OpenGL)

if(NOT TARGET QtNodes::QtNodes)
    include("${QtNodes_CMAKE_DIR}/QtNodesTargets.cmake")
endif()

set(QtNodes_LIBRARIES QtNodes::QtNodes)
