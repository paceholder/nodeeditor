# QML Support for QtNodes

This document describes the implementation of QML support for the **QtNodes** library. This feature allows developers to build modern, hardware-accelerated node editor interfaces using Qt Quick/QML while leveraging the robust C++ graph logic of QtNodes.

## Architecture

The implementation follows a Model-View-ViewModel (MVVM) pattern adapted for Qt/QML:

### 1. C++ Integration Layer (`src/qml/`)
*   **`QuickGraphModel`**: The main controller class. It wraps the internal `DataFlowGraphModel` and exposes high-level operations (add/remove nodes, create connections) to QML.
*   **`NodesListModel`**: A `QAbstractListModel` that exposes the nodes in the graph. It provides roles for properties like position, caption, and input/output port counts. Crucially, it exposes the underlying `NodeDelegateModel` as a `QObject*`, allowing QML to bind directly to custom node data (e.g., numbers, text).
*   **`ConnectionsListModel`**: A `QAbstractListModel` that tracks active connections, providing source/destination node IDs and port indices.

### 2. QML Components (`resources/qml/`)
*   **`NodeGraph.qml`**: The main canvas component.
    *   Handles **Infinite Panning & Zooming**.
    *   Renders a dynamic **Grid** using standard Canvas drawing.
    *   Manages the lifecycle of Nodes and Connections using `Repeater`s.
    *   Handles **Connection Drafting**: Drawing a temporary line while the user drags from a port.
*   **`Node.qml`**: A generic node shell.
    *   Displays the node caption and background.
    *   Generates input/output ports dynamically.
    *   Uses a `Loader` with a `nodeContentDelegate` to allow users to inject **custom QML content** inside the node (e.g., text fields, images).
    *   Handles node dragging and position updates.
*   **`Connection.qml`**:
    *   Renders connections as smooth cubic Bezier curves using `QtQuick.Shapes`.
    *   Updates geometry in real-time when linked nodes are moved.

## Features Implemented

*   ✅ **Hybrid C++/QML Architecture**: Full separation of graph logic (C++) and UI (QML).
*   ✅ **Dynamic Graph Rendering**: Nodes and connections appear and update automatically based on the C++ model.
*   ✅ **Interactive Workspace**: Smooth zooming and panning of the graph canvas.
*   ✅ **Node Manipulation**: Drag-and-drop nodes to move them.
*   ✅ **Connection Creation**: Drag from any port to a compatible target port to create a connection.
*   ✅ **Customizable Nodes**: Users can define the look and behavior of specific node types (e.g., "NumberSource") completely in QML.
*   ✅ **Example Application**: `qml_calculator` demonstrates a working calculator where C++ handles the math and QML handles the UI.

## How to Build

1.  Ensure you have Qt 5.15+ or Qt 6 installed with the **Qt Quick** and **Qt Quick Controls 2** modules.
2.  Run CMake with the `BUILD_QML` flag:

```bash
mkdir build && cd build
cmake .. -DBUILD_QML=ON
make
```

3.  Run the example:
```bash
./bin/qml_calculator
```

## Next Steps (Roadmap)

To achieve full feature parity with the Widgets-based version, the following features need to be implemented:

1.  **Connection Interaction**:
    *   Ability to select/highlight existing connections.
    *   Ability to delete connections (e.g., via right-click menu or keyboard shortcut).
2.  **Node Deletion**:
    *   UI mechanism to delete selected nodes.
3.  **Selection Model**:
    *   Support for selecting multiple nodes (marquee selection).
    *   Visual feedback for selected states.
4.  **Undo/Redo Stack**:
    *   Expose the C++ `UndoStack` to QML to trigger undo/redo actions.
5.  **Styling**:
    *   Expose more style properties (colors, line thickness) to QML for easy theming.
6.  **Port Data & Type Safety**:
    *   Visualize port data types (colors based on type).
    *   Add visual feedback during connection dragging (highlight compatible ports, dim incompatible ones).
