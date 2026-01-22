# QML Support for QtNodes

This document describes the implementation of QML support for the **QtNodes** library. This feature allows developers to build modern, hardware-accelerated node editor interfaces using Qt Quick/QML while leveraging the robust C++ graph logic of QtNodes.

## Architecture

The implementation follows a Model-View-ViewModel (MVVM) pattern adapted for Qt/QML:

### 1. C++ Integration Layer (`src/qml/`)
*   **`QuickGraphModel`**: The main controller class. It wraps the internal `DataFlowGraphModel` and exposes high-level operations (add/remove nodes, create connections) to QML. It also manages an **UndoStack** for undo/redo operations.
*   **`NodesListModel`**: A `QAbstractListModel` that exposes the nodes in the graph. It provides roles for properties like position, caption, and input/output port counts. Crucially, it exposes the underlying `NodeDelegateModel` as a `QObject*`, allowing QML to bind directly to custom node data (e.g., numbers, text).
*   **`ConnectionsListModel`**: A `QAbstractListModel` that tracks active connections, providing source/destination node IDs and port indices.

### 2. QML Components (`resources/qml/`)
*   **`NodeGraph.qml`**: The main canvas component.
    *   Handles **Infinite Panning & Zooming** (mouse-centered) using a background `MouseArea` and transform/scale logic.
    *   Renders a dynamic **Infinite Grid** using a `Canvas` item (avoiding shader compatibility issues).
    *   Manages the lifecycle of Nodes and Connections using `Repeater`s linked to the C++ models.
    *   Handles **Connection Drafting**: Implements geometry-based hit-testing to reliably find target nodes/ports under the mouse cursor, ignoring z-order overlays.
    *   Supports **Marquee Selection** for selecting multiple nodes and connections.
    *   Handles **Keyboard Shortcuts**: Delete/Backspace/X for deletion, Ctrl+Z for undo, Ctrl+Shift+Z/Ctrl+Y for redo.
*   **`Node.qml`**: A generic node shell.
    *   Displays the node caption and background.
    *   Generates input/output ports dynamically with **type-based coloring**.
    *   Uses a `Loader` with a `nodeContentDelegate` to allow users to inject **custom QML content** inside the node (e.g., text fields, images) with full property binding propagation.
    *   Handles node dragging and position updates, including **group dragging** for selected nodes.
    *   Shows visual feedback for selected state.
*   **`Connection.qml`**:
    *   Renders connections as smooth cubic Bezier curves using `QtQuick.Shapes`.
    *   Updates geometry in real-time when linked nodes are moved.
    *   Supports **selection** (click or Ctrl+click) and **hover highlighting**.
    *   Uses **port type colors** for visual consistency.
*   **`NodeGraphStyle.qml`**: A centralized styling component for theming.
    *   Defines colors, sizes, and appearance for canvas, nodes, ports, connections, and selection.
    *   Supports **custom themes** (e.g., dark/light mode) by instantiating with different property values.
    *   Includes port type color mapping for type safety visualization.

## Features Implemented

### Core Functionality
*   ✅ **Hybrid C++/QML Architecture**: Full separation of graph logic (C++) and UI (QML).
*   ✅ **Dynamic Graph Rendering**: Nodes and connections appear and update automatically based on the C++ model.
*   ✅ **Interactive Workspace**: Smooth zooming (mouse-centered) and panning of the graph canvas.
*   ✅ **Node Manipulation**: Drag-and-drop nodes to move them.
*   ✅ **Connection Creation**: Drag from any port to a compatible target port to create a connection.
*   ✅ **Customizable Nodes**: Users can define the look and behavior of specific node types completely in QML.

### Selection & Editing
*   ✅ **Node Selection**: Click to select, Ctrl+click for additive selection.
*   ✅ **Marquee Selection**: Click and drag on canvas to select multiple nodes and connections.
*   ✅ **Group Dragging**: Drag any selected node to move all selected nodes together.
*   ✅ **Connection Selection**: Click on connections to select them, with hover highlighting.
*   ✅ **Node Deletion**: Delete selected nodes via Delete/Backspace/X keys.
*   ✅ **Connection Deletion**: Delete selected connections via Delete/Backspace/X keys.
*   ✅ **Disconnect by Dragging**: Drag from an input port to disconnect and re-route an existing connection.

### Type Safety & Visual Feedback
*   ✅ **Port Type Colors**: Ports are colored based on their data type (decimal=green, integer=blue, string=orange, boolean=purple).
*   ✅ **Compatibility Highlighting**: During connection dragging, compatible ports are highlighted while incompatible ports are dimmed.
*   ✅ **Connection Type Colors**: Connections inherit the color of their source port type.

### Theming & Styling
*   ✅ **NodeGraphStyle.qml**: Centralized styling with customizable properties for:
    *   Canvas background and grid colors
    *   Node background, border, caption, and selection colors
    *   Port sizes, colors, and hover/active states
    *   Connection width, hover effects, and selection outline
    *   Marquee selection appearance
*   ✅ **Theme Switching**: Support for runtime theme changes (e.g., dark/light mode toggle).
*   ✅ **Reactive Styling**: All components respond to style property changes in real-time.

### Undo/Redo
*   ✅ **Full Undo/Redo Support**: All graph operations are undoable:
    *   Add/Remove nodes
    *   Add/Remove connections
*   ✅ **Keyboard Shortcuts**: Ctrl+Z (undo), Ctrl+Shift+Z or Ctrl+Y (redo).
*   ✅ **QML API**: `canUndo`/`canRedo` properties and `undo()`/`redo()` methods exposed to QML.

### Focus Management
*   ✅ **Correct Input Focus**: Text fields inside nodes properly receive and release focus.
*   ✅ **Canvas Focus**: Clicking on canvas or nodes removes focus from inputs for keyboard shortcuts to work.

## Example Application

The `qml_calculator` example demonstrates all features:
*   Multiple node types: NumberSource, Addition, Subtract, Multiply, Divide, FormatNumber, StringDisplay, IntegerSource, ToInteger, GreaterThan, NumberDisplay, IntegerDisplay, BooleanDisplay
*   **Theme Toggle Button**: Switch between dark and light themes at runtime
*   **Undo/Redo Buttons**: Visual buttons in toolbar with enabled/disabled state
*   **Custom Node Content**: Each node type has its own QML UI (text fields, labels, symbols)
*   **Type-Safe Connections**: Connections enforce type compatibility with visual feedback

## Technical Notes

*   **Grid Implementation**: The grid is drawn using an HTML5-style `Canvas` API rather than GLSL shaders. This ensures compatibility with Qt 6's RHI (which removed inline OpenGL shaders) while maintaining performance for infinite grid rendering.
*   **Z-Ordering & Hit Testing**: Custom geometry-based hit testing is used for connection drafting because the temporary connection line (a `Shape` item) overlays the nodes, blocking standard `childAt` calls.
*   **Coordinate Mapping**: All drag operations use `mapToItem`/`mapFromItem` relative to the main `canvas` item to ensure correct positioning regardless of the current pan/zoom state.
*   **Reactive Bindings**: Style properties use direct `graph.style` access for proper reactivity when themes change.
*   **Undo Commands**: Custom `QUndoCommand` subclasses handle node state serialization for proper undo/redo of node additions and deletions.

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

## API Reference

### QuickGraphModel (C++ → QML)

| Property/Method | Type | Description |
|-----------------|------|-------------|
| `nodes` | NodesListModel* | List model of all nodes |
| `connections` | ConnectionsListModel* | List model of all connections |
| `canUndo` | bool | Whether undo is available |
| `canRedo` | bool | Whether redo is available |
| `addNode(nodeType)` | int | Add a node, returns node ID |
| `removeNode(nodeId)` | bool | Remove a node |
| `addConnection(...)` | void | Create a connection |
| `removeConnection(...)` | void | Remove a connection |
| `undo()` | void | Undo last operation |
| `redo()` | void | Redo last undone operation |

### NodeGraph.qml

| Property | Type | Description |
|----------|------|-------------|
| `graphModel` | QuickGraphModel | The C++ model to visualize |
| `style` | NodeGraphStyle | Styling configuration |
| `nodeContentDelegate` | Component | Custom content for nodes |

### NodeGraphStyle.qml

| Category | Properties |
|----------|------------|
| Canvas | `canvasBackground`, `gridMinorLine`, `gridMajorLine`, `gridMinorSpacing`, `gridMajorSpacing` |
| Node | `nodeBackground`, `nodeBorder`, `nodeSelectedBorder`, `nodeBorderWidth`, `nodeSelectedBorderWidth`, `nodeRadius`, `nodeCaptionColor`, `nodeCaptionFontSize`, `nodeCaptionBold`, `nodeMinWidth`, `nodePortSpacing`, `nodeHeaderHeight`, `nodeContentColor` |
| Ports | `portSize`, `portBorderWidth`, `portBorderColor`, `portHighlightBorder`, `portHighlightBorderWidth`, `portHoverScale`, `portActiveScale`, `portDimmedOpacity`, `portTypeColors` |
| Connection | `connectionWidth`, `connectionHoverWidth`, `connectionSelectedWidth`, `connectionSelectionOutline`, `connectionSelectionOutlineWidth`, `draftConnectionWidth`, `draftConnectionColor` |
| Selection | `selectionRectFill`, `selectionRectBorder`, `selectionRectBorderWidth` |

## Keyboard Shortcuts

| Shortcut | Action |
|----------|--------|
| Delete / Backspace / X | Delete selected nodes and connections |
| Ctrl+Z | Undo |
| Ctrl+Shift+Z / Ctrl+Y | Redo |
| Ctrl+Click | Additive selection |

## Future Enhancements

The QML implementation now has feature parity with the Widgets version. Potential future enhancements:

*   **Copy/Paste**: Clipboard support for nodes and connections
*   **Node Groups**: Collapsible node groups for complex graphs
*   **Minimap**: Overview navigation for large graphs
*   **Animation**: Smooth transitions for node/connection state changes
*   **Touch Support**: Multi-touch gestures for mobile/tablet devices
