Core Concepts
=============

Understanding QtNodes' architecture helps you make the right design decisions
for your application.

Model-View Architecture
-----------------------

QtNodes follows Qt's Model-View pattern. Your data (the **Model**) is separate
from its visualization (the **View**).

.. image:: /_static/diagrams/architecture-diagram.png
   :alt: Model-View architecture diagram
   :align: center
   :width: 600px

..
   SCREENSHOT NEEDED: architecture-diagram.png
   Create a simple diagram showing:

   +------------------+          +----------------------+
   |  AbstractGraph   |  ------> | BasicGraphicsScene   |
   |     Model        |          | (QGraphicsScene)     |
   +------------------+          +----------------------+
          ^                               |
          |                               v
      Your Data                  +----------------------+
      (nodes, connections,       |   GraphicsView       |
       positions, etc.)          |   (QGraphicsView)    |
                                 +----------------------+

   Use boxes and arrows. Can be created in any diagramming tool.
   Size: ~600px wide

**Benefits of this separation:**

- Run graph logic without any GUI (headless mode)
- Multiple views of the same data
- Clear ownership: you own your data, the library owns the visuals
- Easy testing of graph logic

Nodes and Connections
---------------------

Graphs consist of **nodes** and **connections**.

.. image:: /_static/screenshots/node-anatomy.png
   :alt: Anatomy of a node
   :align: center
   :width: 400px

**Node identification:**

- Each node has a unique ``NodeId`` (unsigned integer)
- You generate IDs in your model via ``newNodeId()``
- The library never creates or manages IDs for you

**Connections:**

- A ``ConnectionId`` links an output port to an input port
- Contains: ``outNodeId``, ``outPortIndex``, ``inNodeId``, ``inPortIndex``
- Connections are directional: data flows from Out to In

.. code-block:: cpp

   // A connection from Node 1, Port 0 -> Node 2, Port 0
   ConnectionId conn{
       .outNodeId = 1,
       .outPortIndex = 0,
       .inNodeId = 2,
       .inPortIndex = 0
   };

The Two Programming Models
--------------------------

QtNodes supports two distinct approaches:

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - **Simple Graph Model**
     - **Data Flow Model**
   * - You subclass ``AbstractGraphModel``
     - You use ``DataFlowGraphModel`` + delegates
   * - You store all graph data yourself
     - Library manages node instances for you
   * - No automatic data propagation
     - Data flows automatically through connections
   * - Full control, more code
     - Less code, follows conventions
   * - ``BasicGraphicsScene``
     - ``DataFlowGraphicsScene``

**Choose Simple Graph Model when:**

- You have existing graph data structures to wrap
- You need custom graph semantics
- You don't need data to flow between nodes

**Choose Data Flow Model when:**

- Building visual programming tools
- Nodes process inputs and produce outputs
- You want automatic data propagation

NodeRole and PortRole
---------------------

The model provides data about nodes and ports through **roles** (similar to
Qt's ``ItemDataRole``).

**NodeRole** -- Information about a node:

.. code-block:: cpp

   // Get node position
   QPointF pos = model.nodeData(nodeId, NodeRole::Position).toPointF();

   // Set node caption
   model.setNodeData(nodeId, NodeRole::Caption, "My Node");

Key roles: ``Type``, ``Position``, ``Caption``, ``InPortCount``, ``OutPortCount``, ``Widget``

**PortRole** -- Information about a port:

.. code-block:: cpp

   // Get port data type
   auto type = model.portData(nodeId, PortType::In, 0, PortRole::DataType);

Key roles: ``Data``, ``DataType``, ``Caption``, ``ConnectionPolicyRole``

See :doc:`/guide/graph-models` for the complete role reference.

Graphics Object Hierarchy
-------------------------

When visualized, each model element becomes a graphics object:

.. code-block:: text

   GraphicsView (QGraphicsView)
     └── BasicGraphicsScene (QGraphicsScene)
           ├── NodeGraphicsObject (for each node)
           │     └── Embedded QWidget (optional)
           └── ConnectionGraphicsObject (for each connection)

**Painters** control how objects are drawn:

- ``AbstractNodePainter`` → ``DefaultNodePainter``
- ``AbstractConnectionPainter`` → ``DefaultConnectionPainter``

You can replace these with custom painters for different visual styles.

Data Flow Deep Dive
-------------------

In the data flow model, delegates handle node-specific logic:

.. image:: /_static/diagrams/dataflow-diagram.png
   :alt: Data flow between nodes
   :align: center
   :width: 500px

..
   SCREENSHOT NEEDED: dataflow-diagram.png
   Show data flowing through connected nodes:

   [Source Node]  -->  [Operator Node]  -->  [Display Node]
       |                    |                     |
   "emits data"      "receives, processes,    "receives,
                      emits result"            displays"

   Use arrows showing dataUpdated signal flow.
   Size: ~500px wide

**The flow:**

1. Source node calls ``emit dataUpdated(portIndex)``
2. ``DataFlowGraphModel`` catches this signal
3. It calls ``outData(port)`` on the source delegate
4. It calls ``setInData(data, port)`` on all connected delegates
5. Connected nodes process and may emit their own ``dataUpdated``

Next Steps
----------

- :doc:`/guide/graph-models` -- Implement your own graph model
- :doc:`/guide/data-flow` -- Use the data flow pattern
- :doc:`/examples/index` -- Study working examples
