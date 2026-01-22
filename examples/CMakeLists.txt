add_subdirectory(simple_graph_model)

add_subdirectory(vertical_layout)

add_subdirectory(calculator)

add_subdirectory(text)

add_subdirectory(resizable_images)

add_subdirectory(styles)

add_subdirectory(connection_colors)

add_subdirectory(dynamic_ports)

add_subdirectory(lock_nodes_and_connections)

if(Qt${QT_VERSION_MAJOR}Quick_FOUND AND ${QT_VERSION} VERSION_GREATER_EQUAL 6.0.0)
  add_subdirectory(calculator_qml)
endif()
