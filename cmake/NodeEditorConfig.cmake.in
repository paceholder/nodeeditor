get_filename_component(NodeEditor_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" PATH)

include(CMakeFindDependencyMacro)

# NOTE Had to use find_package because find_dependency does not support COMPONENTS or MODULE until 3.8.0

find_package(Qt5 REQUIRED COMPONENTS
             Core
             Widgets
             Gui
             OpenGL)

if(NOT TARGET NodeEditor::nodes)
    include("${NodeEditor_CMAKE_DIR}/NodeEditorTargets.cmake")
endif()

set(NodeEditor_LIBRARIES NodeEditor::nodes)
