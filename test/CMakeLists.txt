if (Qt6_FOUND)
  find_package(Qt6 COMPONENTS Test)
  set(Qt Qt)
else()
  find_package(Qt5 COMPONENTS Test)
  set(Qt Qt5)
endif()

add_executable(test_nodes
  test_main.cpp
  src/TestDragging.cpp
  src/TestDataModelRegistry.cpp
  src/TestFlowScene.cpp
  src/TestNodeGraphicsObject.cpp
  include/ApplicationSetup.hpp
  include/Stringify.hpp
  include/StubNodeDataModel.hpp
)

target_include_directories(test_nodes
  PRIVATE
    ../src
    ../include/internal
    include
)

target_link_libraries(test_nodes
  PRIVATE
    QtNodes::QtNodes
    Catch2::Catch2
    ${Qt}::Test
)

add_test(
  NAME test_nodes
  COMMAND
    $<TARGET_FILE:test_nodes>
    $<$<BOOL:${NE_FORCE_TEST_COLOR}>:--use-colour=yes>
)
