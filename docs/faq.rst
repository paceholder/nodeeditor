FAQ & Troubleshooting
=====================

Frequently asked questions and common issues.

General Questions
-----------------

**Which model should I use?**

- Use ``DataFlowGraphModel`` + ``NodeDelegateModel`` if you want automatic
  data propagation between nodes (visual programming, calculators, pipelines).

- Use a custom ``AbstractGraphModel`` subclass if you have existing graph
  data structures or need custom graph semantics.

**Can I use QtNodes without a GUI?**

Yes. The model classes (``AbstractGraphModel``, ``DataFlowGraphModel``) work
without any scene or view. This is called "headless mode."

.. code-block:: cpp

   DataFlowGraphModel model(registry);
   NodeId n1 = model.addNode("Source");
   NodeId n2 = model.addNode("Display");
   model.addConnection({n1, 0, n2, 0});
   // Data flows, no GUI needed

**Does QtNodes support Qt5 and Qt6?**

Yes. Set ``-DUSE_QT6=OFF`` for Qt5, or ``-DUSE_QT6=ON`` (default) for Qt6.

Common Issues
-------------

**Nodes don't appear after adding them**

Make sure you emit the ``nodeCreated`` signal:

.. code-block:: cpp

   NodeId MyModel::addNode(QString type) override
   {
       NodeId id = newNodeId();
       _nodes.insert(id);
       emit nodeCreated(id);  // Don't forget this!
       return id;
   }

**Connections disappear or don't work**

Check that:

1. ``connectionPossible()`` returns ``true`` for valid connections
2. ``connectionExists()`` correctly checks your data structure
3. You emit ``connectionCreated`` after adding a connection

**Undo doesn't restore deleted nodes**

Implement ``saveNode()`` and ``loadNode()`` in your model:

.. code-block:: cpp

   QJsonObject MyModel::saveNode(NodeId nodeId) const override
   {
       // Return all data needed to recreate this node
   }

   void MyModel::loadNode(QJsonObject const& json) override
   {
       // Recreate node from saved data
   }

**Embedded widget doesn't show**

Ensure your ``embeddedWidget()`` returns a valid widget:

.. code-block:: cpp

   QWidget* MyNode::embeddedWidget() override
   {
       if (!_widget) {
           _widget = new QLineEdit();  // Create lazily
       }
       return _widget;
   }

**Data doesn't propagate to connected nodes**

Make sure you emit ``dataUpdated`` when output changes:

.. code-block:: cpp

   void MyNode::compute()
   {
       _result = doComputation();
       emit dataUpdated(0);  // Notify downstream nodes
   }

**Crash when deleting nodes**

Delete connections before deleting the node:

.. code-block:: cpp

   bool MyModel::deleteNode(NodeId nodeId) override
   {
       // Remove connections first
       for (auto& conn : allConnectionIds(nodeId)) {
           deleteConnection(conn);
       }

       _nodes.erase(nodeId);
       emit nodeDeleted(nodeId);
       return true;
   }

Build Issues
------------

**Catch2 not found**

Either install Catch2 or disable testing:

.. code-block:: bash

   cmake .. -DBUILD_TESTING=OFF

**Qt version conflicts**

Make sure all components use the same Qt version. Check with:

.. code-block:: bash

   cmake .. -DUSE_QT6=ON   # or OFF for Qt5

**Linking errors on Windows**

Ensure you're using the correct build type:

.. code-block:: bash

   cmake .. -DCMAKE_BUILD_TYPE=Release
   cmake --build . --config Release

Performance Questions
---------------------

**How many nodes can QtNodes handle?**

QtNodes has been used with hundreds of nodes. For very large graphs (1000+),
consider:

- Using ``BasicGraphicsScene`` instead of ``DataFlowGraphicsScene``
- Implementing virtualization (only create graphics for visible nodes)
- Disabling shadows and complex styles

**Why is my graph slow to render?**

Check for:

- Complex embedded widgets in every node
- Heavy computation in ``paint()`` methods
- Many connections causing recalculation

Feature Questions
-----------------

**Can I have ports on all four sides?**

The built-in geometries support horizontal (left/right) or vertical (top/bottom).
For ports on all sides, create a custom ``AbstractNodeGeometry``.

**Can I group nodes?**

Node grouping is planned but not yet implemented. Track progress on GitHub.

**Can I have curved connections?**

Yes, the default connection painter draws cubic bezier curves. For custom
curves, create a custom ``AbstractConnectionPainter``.

**Can I add custom context menus?**

Override ``createSceneMenu()`` in your scene subclass:

.. code-block:: cpp

   QMenu* MyScene::createSceneMenu(QPointF scenePos) override
   {
       auto* menu = new QMenu();
       menu->addAction("Custom Action", [=]() { ... });
       return menu;
   }

Getting Help
------------

- **Documentation**: You're reading it!
- **Examples**: Check ``examples/`` for working code
- **Issues**: `GitHub Issues <https://github.com/paceholder/nodeeditor/issues>`_
- **Discussions**: `GitHub Discussions <https://github.com/paceholder/nodeeditor/discussions>`_
