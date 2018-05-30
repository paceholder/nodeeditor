find_package(Catch2 REQUIRED)
find_package(Qt5 COMPONENTS Test)

add_executable(test_nodes
  test_main.cpp
  src/test_dragging.cpp
  src/TestDataModelRegistry.cpp
  src/TestNodeGraphicsObject.cpp
)

target_include_directories(test_nodes
  PRIVATE
    ../src
    ../include/internal
    include
)

target_link_libraries(test_nodes
  PRIVATE
    NodeEditor::nodes
    Catch2::Catch
    Qt5::Test
)

add_test(test_nodes test_nodes)
