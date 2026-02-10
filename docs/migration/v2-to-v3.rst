Migrating from v2.x to v3.x
===========================

Version 3.0 introduced a Model-View architecture. This guide helps you
migrate existing code.

Overview of Changes
-------------------

**Architecture shift:**

- v2: ``Node`` and ``Connection`` objects held their own data
- v3: ``AbstractGraphModel`` holds all data; graphics objects are just views

**Key benefits of v3:**

- Headless operation (no GUI required)
- Multiple views of same data
- Better separation of concerns
- Easier testing

Class Renames
-------------

.. list-table::
   :widths: 25 25 50
   :header-rows: 1

   * - v2.x Class
     - v3.x Class
     - Notes
   * - ``Node``
     - (removed)
     - Nodes are now just ``NodeId`` values
   * - ``Connection``
     - (removed)
     - Connections are now ``ConnectionId`` structs
   * - ``NodeDataModel``
     - ``NodeDelegateModel``
     - Per-node logic is now a "delegate"
   * - ``DataModelRegistry``
     - ``NodeDelegateModelRegistry``
     - Consistent naming
   * - ``FlowView``
     - ``GraphicsView``
     - Simpler name
   * - ``FlowScene``
     - ``DataFlowGraphicsScene``
     - Inherits from ``BasicGraphicsScene``
   * - ``NodePainter``
     - ``DefaultNodePainter``
     - Now inherits ``AbstractNodePainter``
   * - (new)
     - ``AbstractGraphModel``
     - Central data store
   * - (new)
     - ``DataFlowGraphModel``
     - Model with data propagation

Migration Steps
---------------

**Step 1: Replace FlowScene/FlowView**

Before (v2):

.. code-block:: cpp

   FlowScene* scene = new FlowScene(registry);
   FlowView* view = new FlowView(scene);

After (v3):

.. code-block:: cpp

   DataFlowGraphModel model(registry);
   DataFlowGraphicsScene* scene = new DataFlowGraphicsScene(model);
   GraphicsView* view = new GraphicsView(scene);

**Step 2: Rename NodeDataModel to NodeDelegateModel**

Before:

.. code-block:: cpp

   class MyNode : public NodeDataModel { ... };

After:

.. code-block:: cpp

   class MyNode : public NodeDelegateModel { ... };

**Step 3: Update registry usage**

Before:

.. code-block:: cpp

   auto registry = std::make_shared<DataModelRegistry>();
   registry->registerModel<MyNode>();

After:

.. code-block:: cpp

   auto registry = std::make_shared<NodeDelegateModelRegistry>();
   registry->registerModel<MyNode>("Category");  // Category now required

**Step 4: Update node access**

Before (v2 - accessing Node objects):

.. code-block:: cpp

   Node& node = scene->createNode(...);
   node.nodeDataModel()->setData(...);

After (v3 - accessing through model):

.. code-block:: cpp

   NodeId nodeId = model.addNode("MyNode");
   auto* delegate = model.delegateModel<MyNode>(nodeId);
   delegate->setData(...);

API Changes in NodeDelegateModel
--------------------------------

Most methods remain the same:

- ``name()`` -- unchanged
- ``caption()`` -- unchanged
- ``nPorts()`` -- unchanged
- ``dataType()`` -- unchanged
- ``setInData()`` -- unchanged
- ``outData()`` -- unchanged
- ``embeddedWidget()`` -- unchanged

**New methods:**

- ``validationState()`` -- Returns current validation state
- ``processingStatus()`` -- Returns processing status
- ``setValidationState()`` -- Set validation state
- ``setNodeProcessingStatus()`` -- Set processing status

Removed Features
----------------

These v2 features were removed in v3:

**Warning messages at node bottom**

v2 displayed validation messages below nodes. In v3, use ``NodeValidationState``
instead, which shows icons and tooltips.

**Data Type Converters**

v2 had automatic type converters. In v3, handle type compatibility in
``connectionPossible()`` or convert data in your delegate's ``setInData()``.

Connection Changes
------------------

Before (v2):

.. code-block:: cpp

   // Connections were objects
   Connection* conn = scene->createConnection(...);

After (v3):

.. code-block:: cpp

   // Connections are just IDs
   ConnectionId conn{outNodeId, outPortIndex, inNodeId, inPortIndex};
   model.addConnection(conn);

Serialization Changes
---------------------

The JSON format is compatible, but the API changed:

Before (v2):

.. code-block:: cpp

   scene->save();  // QByteArray
   scene->load();  // from file dialog

After (v3):

.. code-block:: cpp

   QJsonObject json = model.save();
   model.load(json);

   // Or use scene helpers:
   scene.save();  // Opens file dialog
   scene.load();  // Opens file dialog

Custom Painters
---------------

Before (v2):

.. code-block:: cpp

   // NodePainter was created on the stack during paint
   // No easy way to customize

After (v3):

.. code-block:: cpp

   class MyPainter : public AbstractNodePainter {
       void paint(QPainter*, NodeGraphicsObject&) const override;
   };

   scene.setNodePainter(std::make_unique<MyPainter>());

Getting Help
------------

If you encounter migration issues:

1. Check the examples in ``examples/`` -- they all use v3 API
2. See the :doc:`/guide/data-flow` guide for data flow patterns
3. Open an issue on `GitHub <https://github.com/paceholder/nodeeditor/issues>`_
