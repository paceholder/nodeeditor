if (USE_QT6)
  find_package(Qt6 COMPONENTS Test)
else()
  find_package(Qt5 COMPONENTS Test)
endif()

add_executable(test_nodes
  test_main.cpp
  src/TestAbstractGraphModel.cpp
  src/TestAbstractGraphModelSignals.cpp
  src/TestDataFlowGraphModel.cpp
  src/TestNodeDelegateModelRegistry.cpp
  src/TestConnectionId.cpp
  src/TestSerialization.cpp
  src/TestUndoCommands.cpp
  src/TestBasicGraphicsScene.cpp
  src/TestNodeGroup.cpp
  src/TestUIInteraction.cpp
  src/TestDataFlow.cpp
  include/ApplicationSetup.hpp
  include/TestGraphModel.hpp
  include/UITestHelper.hpp
  include/TestDataFlowNodes.hpp
)

target_include_directories(test_nodes
  PRIVATE
    include
)

target_link_libraries(test_nodes
  PRIVATE
    QtNodes::QtNodes
    Catch2::Catch2
    Qt${QT_VERSION_MAJOR}::Test
)

add_test(
  NAME test_nodes
  COMMAND
    $<TARGET_FILE:test_nodes>
    $<$<BOOL:${NE_FORCE_TEST_COLOR}>:--use-colour=yes>
)
