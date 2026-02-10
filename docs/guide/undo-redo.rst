Undo/Redo
=========

QtNodes provides built-in undo/redo support using Qt's undo framework.

How It Works
------------

The ``BasicGraphicsScene`` maintains a ``QUndoStack``. User actions
automatically push commands to this stack:

- Creating nodes
- Deleting nodes
- Creating connections
- Deleting connections
- Moving nodes
- Duplicating nodes

Accessing the Undo Stack
------------------------

.. code-block:: cpp

   BasicGraphicsScene scene(model);

   // Get the undo stack
   QUndoStack& undoStack = scene.undoStack();

   // Wire up to UI
   QAction* undoAction = undoStack.createUndoAction(this, "Undo");
   undoAction->setShortcut(QKeySequence::Undo);

   QAction* redoAction = undoStack.createRedoAction(this, "Redo");
   redoAction->setShortcut(QKeySequence::Redo);

   editMenu->addAction(undoAction);
   editMenu->addAction(redoAction);

Built-in Commands
-----------------

QtNodes provides these ``QUndoCommand`` implementations in ``UndoCommands.cpp``:

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Command
     - Description
   * - ``DeleteCommand``
     - Removes nodes and their connections
   * - ``DuplicateCommand``
     - Duplicates selected nodes
   * - ``DisconnectCommand``
     - Removes a connection
   * - ``ConnectCommand``
     - Creates a connection
   * - ``MoveNodeCommand``
     - Moves a node to a new position

Serialization Requirement
-------------------------

Undo/redo for node deletion requires serialization support. Make sure your
model implements ``saveNode()`` and ``loadNode()``:

.. code-block:: cpp

   QJsonObject MyModel::saveNode(NodeId nodeId) const override
   {
       // Save all data needed to recreate this node
       QJsonObject json;
       json["id"] = static_cast<qint64>(nodeId);

       QPointF pos = nodeData(nodeId, NodeRole::Position).toPointF();
       json["position"] = QJsonObject{{"x", pos.x()}, {"y", pos.y()}};

       // Save your custom data too
       json["internal-data"] = getNodeInternalData(nodeId);

       return json;
   }

   void MyModel::loadNode(QJsonObject const& json) override
   {
       // Recreate node from saved data
       NodeId nodeId = static_cast<NodeId>(json["id"].toInt());
       _nextId = std::max(_nextId, nodeId + 1);

       _nodes.insert(nodeId);
       emit nodeCreated(nodeId);

       // Restore position
       auto pos = json["position"].toObject();
       setNodeData(nodeId, NodeRole::Position,
                   QPointF(pos["x"].toDouble(), pos["y"].toDouble()));

       // Restore custom data
       restoreNodeInternalData(nodeId, json["internal-data"].toObject());
   }

.. warning::

   Without proper ``saveNode()``/``loadNode()``, deleted nodes cannot be
   restored by undo.

Custom Undo Commands
--------------------

Create custom commands for your own operations:

.. code-block:: cpp

   class ChangeNodeColorCommand : public QUndoCommand
   {
   public:
       ChangeNodeColorCommand(MyModel* model, NodeId nodeId, QColor newColor)
           : _model(model)
           , _nodeId(nodeId)
           , _newColor(newColor)
           , _oldColor(model->getNodeColor(nodeId))
       {
           setText(QString("Change color of node %1").arg(nodeId));
       }

       void undo() override {
           _model->setNodeColor(_nodeId, _oldColor);
       }

       void redo() override {
           _model->setNodeColor(_nodeId, _newColor);
       }

   private:
       MyModel* _model;
       NodeId _nodeId;
       QColor _newColor;
       QColor _oldColor;
   };

   // Push to stack
   scene.undoStack().push(
       new ChangeNodeColorCommand(&model, nodeId, Qt::red)
   );

Grouping Commands
-----------------

Use macros to group multiple commands:

.. code-block:: cpp

   undoStack.beginMacro("Batch Operation");

   // These will undo/redo together
   undoStack.push(new Command1(...));
   undoStack.push(new Command2(...));
   undoStack.push(new Command3(...));

   undoStack.endMacro();

Clearing History
----------------

.. code-block:: cpp

   // Clear all undo history
   undoStack.clear();

   // Mark current state as "clean" (for save indicators)
   undoStack.setClean();

   // Check if modified since last save
   if (!undoStack.isClean()) {
       // Show "unsaved changes" warning
   }

Keyboard Shortcuts
------------------

Default shortcuts (handled by ``GraphicsView``):

- ``Ctrl+Z`` -- Undo
- ``Ctrl+Shift+Z`` or ``Ctrl+Y`` -- Redo
- ``Delete`` -- Delete selection (creates ``DeleteCommand``)
- ``Ctrl+D`` -- Duplicate (creates ``DuplicateCommand``)

.. seealso::

   - `Qt Undo Framework <https://doc.qt.io/qt-6/qundostack.html>`_
   - :doc:`serialization` -- Required for undo/redo
