Quick Start
===========

This tutorial creates a simple node graph editor in under 50 lines of code.
By the end, you'll have an interactive canvas where you can create nodes,
connect them, and move them around.

.. image:: /_static/screenshots/quickstart-result.png
   :alt: The node graph we'll build in this tutorial
   :align: center

What We're Building
-------------------

A minimal graph editor that:

- Displays nodes on a canvas
- Lets you create new nodes via right-click menu
- Allows connecting nodes by dragging between ports
- Supports selecting, moving, and deleting nodes

Step 1: Create the Graph Model
------------------------------

First, we need a class to store our graph data. Create ``SimpleGraphModel.hpp``:

.. code-block:: cpp

   #pragma once
   #include <QtNodes/AbstractGraphModel>
   #include <unordered_set>
   #include <unordered_map>

   class SimpleGraphModel : public QtNodes::AbstractGraphModel
   {
       Q_OBJECT
   public:
       // Required: Generate unique node IDs
       QtNodes::NodeId newNodeId() override { return _nextId++; }

       // Required: Return all node IDs
       std::unordered_set<QtNodes::NodeId> allNodeIds() const override;

       // Required: Return all connections for a node
       std::unordered_set<QtNodes::ConnectionId> allConnectionIds(QtNodes::NodeId) const override;

       // ... (see full implementation in examples/simple_graph_model)

   private:
       QtNodes::NodeId _nextId = 0;
       std::unordered_set<QtNodes::NodeId> _nodes;
       std::unordered_set<QtNodes::ConnectionId> _connections;
   };

The model inherits from ``AbstractGraphModel`` and stores:

- A set of node IDs
- A set of connections (each connecting two ports)
- Node positions and other metadata

.. tip::

   For the complete implementation, see ``examples/simple_graph_model/SimpleGraphModel.cpp``.
   It's about 150 lines implementing all the required virtual methods.

Step 2: Create the Main Window
------------------------------

In ``main.cpp``:

.. code-block:: cpp

   #include <QApplication>
   #include <QtNodes/GraphicsView>
   #include <QtNodes/BasicGraphicsScene>
   #include "SimpleGraphModel.hpp"

   int main(int argc, char* argv[])
   {
       QApplication app(argc, argv);

       // 1. Create the graph model
       SimpleGraphModel model;

       // 2. Create a scene that visualizes the model
       auto* scene = new QtNodes::BasicGraphicsScene(model);

       // 3. Create a view to display the scene
       QtNodes::GraphicsView view(scene);
       view.setWindowTitle("My First Node Graph");
       view.resize(800, 600);
       view.show();

       return app.exec();
   }

Step 3: Add Node Creation
-------------------------

Let users create nodes via right-click:

.. code-block:: cpp

   // After creating the view...
   view.setContextMenuPolicy(Qt::ActionsContextMenu);

   QAction* createAction = new QAction("Create Node", &view);
   QObject::connect(createAction, &QAction::triggered, [&]() {
       // Get mouse position in scene coordinates
       QPointF pos = view.mapToScene(view.mapFromGlobal(QCursor::pos()));

       // Add node to model
       auto nodeId = model.addNode();
       model.setNodeData(nodeId, QtNodes::NodeRole::Position, pos);
   });

   view.addAction(createAction);

Step 4: Build and Run
---------------------

.. code-block:: cmake

   # CMakeLists.txt
   find_package(QtNodes REQUIRED)

   add_executable(my_first_graph main.cpp SimpleGraphModel.cpp)
   target_link_libraries(my_first_graph QtNodes::QtNodes)

Run it:

.. code-block:: bash

   mkdir build && cd build
   cmake .. && make
   ./my_first_graph

Interacting with Your Graph
---------------------------

.. |icon_create| image:: /_static/screenshots/quickstart-create.png
   :width: 300px

.. |icon_connect| image:: /_static/screenshots/quickstart-connect.png
   :width: 300px

.. |icon_select| image:: /_static/screenshots/quickstart-select.png
   :width: 300px

.. list-table::
   :widths: 33 33 33

   * - |icon_create|
     - |icon_connect|
     - |icon_select|
   * - **Create**: Right-click canvas
     - **Connect**: Drag from port to port
     - **Select**: Click node or drag box

**Keyboard shortcuts:**

- ``Delete`` -- Remove selected nodes/connections
- ``Ctrl+Z`` -- Undo
- ``Ctrl+Shift+Z`` -- Redo
- ``Ctrl+D`` -- Duplicate selection

What's Next?
------------

You've built a basic graph editor! Here's where to go next:

- :doc:`concepts` -- Understand the Model-View architecture
- :doc:`/guide/data-flow` -- Make data flow between nodes
- :doc:`/guide/styling` -- Customize the look and feel
- :doc:`/examples/index` -- Explore complete examples
