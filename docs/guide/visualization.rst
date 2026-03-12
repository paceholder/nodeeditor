Visualization
=============

This guide covers the graphics system: scenes, views, painters, and geometry.

Scene and View
--------------

QtNodes uses Qt's Graphics View Framework:

- **BasicGraphicsScene** -- A ``QGraphicsScene`` that creates visual objects for your model
- **GraphicsView** -- A ``QGraphicsView`` with node-editing interactions built in

.. code-block:: cpp

   // Connect model to scene to view
   MyGraphModel model;
   BasicGraphicsScene scene(model);
   GraphicsView view(&scene);
   view.show();

**For data flow applications**, use the specialized scene:

.. code-block:: cpp

   DataFlowGraphModel model(registry);
   DataFlowGraphicsScene scene(model);  // Handles delegate creation
   GraphicsView view(&scene);

Graphics Objects
----------------

The scene creates graphics objects automatically:

- **NodeGraphicsObject** -- Visual representation of each node
- **ConnectionGraphicsObject** -- Visual representation of each connection

Access them when needed:

.. code-block:: cpp

   NodeGraphicsObject* nodeObj = scene.nodeGraphicsObject(nodeId);
   ConnectionGraphicsObject* connObj = scene.connectionGraphicsObject(connId);

Custom Node Painter
-------------------

Replace the default node appearance by subclassing ``AbstractNodePainter``:

.. code-block:: cpp

   class MyNodePainter : public QtNodes::AbstractNodePainter
   {
   public:
       void paint(QPainter* painter, NodeGraphicsObject& ngo) const override
       {
           auto& geometry = ngo.nodeScene()->nodeGeometry();
           auto& model = ngo.graphModel();
           NodeId nodeId = ngo.nodeId();

           QRectF rect = geometry.boundingRect(nodeId);

           // Custom drawing
           painter->setBrush(QColor(70, 130, 180));
           painter->setPen(QPen(Qt::white, 2));
           painter->drawRoundedRect(rect, 10, 10);

           // Draw caption
           QString caption = model.nodeData(nodeId, NodeRole::Caption).toString();
           painter->drawText(rect, Qt::AlignCenter, caption);
       }
   };

   // Register with scene
   scene.setNodePainter(std::make_unique<MyNodePainter>());

.. image:: /_static/screenshots/custom-painter.png
   :alt: Nodes with custom painting
   :width: 400px

..
   SCREENSHOT NEEDED: custom-painter.png
   - Show the custom_painter example running
   - Display nodes with the blue-purple gradient style
   - Show connections with arrows
   - Size: ~400px wide

Custom Connection Painter
-------------------------

Similarly for connections:

.. code-block:: cpp

   class MyConnectionPainter : public QtNodes::AbstractConnectionPainter
   {
   public:
       void paint(QPainter* painter, ConnectionGraphicsObject const& cgo) const override
       {
           QPointF start = cgo.endPoint(PortType::Out);
           QPointF end = cgo.endPoint(PortType::In);

           // Draw as straight line with arrow
           painter->setPen(QPen(Qt::cyan, 3));
           painter->drawLine(start, end);

           // Draw arrow head at end
           drawArrow(painter, end, start);
       }

       QPainterPath getPainterStroke(ConnectionGraphicsObject const& cgo) const override
       {
           // Return path for hit testing
           QPainterPath path;
           path.moveTo(cgo.endPoint(PortType::Out));
           path.lineTo(cgo.endPoint(PortType::In));

           QPainterPathStroker stroker;
           stroker.setWidth(10);
           return stroker.createStroke(path);
       }
   };

   scene.setConnectionPainter(std::make_unique<MyConnectionPainter>());

Custom Node Geometry
--------------------

Control node layout by subclassing ``AbstractNodeGeometry``:

.. code-block:: cpp

   class MyNodeGeometry : public QtNodes::AbstractNodeGeometry
   {
   public:
       QRectF boundingRect(NodeId nodeId) const override;
       QPointF portPosition(NodeId, PortType, PortIndex) const override;
       QPointF captionPosition(NodeId) const override;
       QPointF widgetPosition(NodeId) const override;
       // ... other layout methods
   };

   scene.setNodeGeometry(std::make_unique<MyNodeGeometry>());

The library provides two built-in geometries:

- ``DefaultHorizontalNodeGeometry`` -- Ports on left/right (default)
- ``DefaultVerticalNodeGeometry`` -- Ports on top/bottom

Vertical Layout
---------------

For top-to-bottom graphs:

.. code-block:: cpp

   scene.setOrientation(Qt::Vertical);

.. image:: /_static/screenshots/vertical-layout.png
   :alt: Vertical node layout
   :width: 500px

..
   SCREENSHOT NEEDED: vertical-layout.png
   - Show vertical_layout example
   - Nodes with ports on top and bottom
   - Connections flowing downward
   - Size: ~300px wide

GraphicsView Features
---------------------

**Zoom control:**

.. code-block:: cpp

   // Set zoom limits (0 = unlimited)
   view.setScaleRange(0.25, 4.0);

   // Zoom programmatically
   view.scaleUp();
   view.scaleDown();
   view.setupScale(1.5);

   // Fit content
   view.zoomFitAll();       // Fit all nodes
   view.zoomFitSelected();  // Fit selected nodes

   // React to zoom changes
   connect(&view, &GraphicsView::scaleChanged, [](double scale) {
       qDebug() << "Zoom:" << scale * 100 << "%";
   });

**Built-in actions:**

.. code-block:: cpp

   view.clearSelectionAction();   // Clear selection
   view.deleteSelectionAction();  // Delete selected items

**Copy/paste:**

.. code-block:: cpp

   view.onCopySelectedObjects();
   view.onPasteObjects();
   view.onDuplicateSelectedObjects();  // Ctrl+D

Scene Signals
-------------

Connect to user interactions:

.. code-block:: cpp

   // Node interactions
   connect(&scene, &BasicGraphicsScene::nodeClicked, [](NodeId id) {
       qDebug() << "Clicked node" << id;
   });

   connect(&scene, &BasicGraphicsScene::nodeDoubleClicked, [](NodeId id) {
       // Open node properties dialog
   });

   connect(&scene, &BasicGraphicsScene::nodeMoved, [](NodeId id, QPointF pos) {
       qDebug() << "Node" << id << "moved to" << pos;
   });

   // Hover events (for tooltips, highlights)
   connect(&scene, &BasicGraphicsScene::nodeHovered,
           [](NodeId id, QPoint screenPos) {
       QToolTip::showText(screenPos, "Node info here");
   });

   connect(&scene, &BasicGraphicsScene::connectionHovered,
           [](ConnectionId id, QPoint screenPos) {
       // Show connection info
   });

   // Context menus
   connect(&scene, &BasicGraphicsScene::nodeContextMenu,
           [](NodeId id, QPointF scenePos) {
       // Show custom context menu for this node
   });

Context Menus
-------------

Override ``createSceneMenu()`` for custom right-click menus:

.. code-block:: cpp

   class MyScene : public DataFlowGraphicsScene
   {
   public:
       QMenu* createSceneMenu(QPointF scenePos) override
       {
           auto* menu = new QMenu();

           for (auto& category : _registry->categories()) {
               auto* submenu = menu->addMenu(category);
               for (auto& modelName : _registry->registeredModelsByCategory(category)) {
                   submenu->addAction(modelName, [=]() {
                       // Create node at scenePos
                   });
               }
           }

           return menu;
       }
   };

Headless Mode
-------------

Run without any visualization:

.. code-block:: cpp

   // Model works without scene
   DataFlowGraphModel model(registry);

   NodeId source = model.addNode("NumberSource");
   NodeId display = model.addNode("Display");
   model.addConnection({source, 0, display, 0});

   // Data flows, computations happen, no GUI needed

   // Or: scene without view
   BasicGraphicsScene scene(model);
   // Scene tracks model changes but nothing is rendered

.. seealso::

   - :doc:`styling` -- Customize colors and styles
   - :doc:`/examples/index` -- Visual examples
