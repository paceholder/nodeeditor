Serialization
=============

Save and load graphs as JSON.

JSON Structure
--------------

A saved graph contains nodes and connections:

.. code-block:: json

   {
     "nodes": [
       {
         "id": 0,
         "position": {"x": 100, "y": 50},
         "internal-data": {
           "model-name": "NumberSource",
           "value": 42.0
         }
       },
       {
         "id": 1,
         "position": {"x": 300, "y": 50},
         "internal-data": {
           "model-name": "Display"
         }
       }
     ],
     "connections": [
       {
         "outNodeId": 0,
         "outPortIndex": 0,
         "inNodeId": 1,
         "inPortIndex": 0
       }
     ]
   }

Using DataFlowGraphModel
------------------------

``DataFlowGraphModel`` implements ``Serializable``:

.. code-block:: cpp

   DataFlowGraphModel model(registry);

   // Save to JSON object
   QJsonObject json = model.save();

   // Save to file
   QFile file("graph.json");
   file.open(QIODevice::WriteOnly);
   file.write(QJsonDocument(json).toJson());

   // Load from file
   file.open(QIODevice::ReadOnly);
   QJsonObject loadedJson = QJsonDocument::fromJson(file.readAll()).object();
   model.load(loadedJson);

Using DataFlowGraphicsScene
---------------------------

The scene provides file dialogs:

.. code-block:: cpp

   DataFlowGraphicsScene scene(model);

   // Opens save dialog, returns true on success
   if (scene.save()) {
       qDebug() << "Saved!";
   }

   // Opens load dialog
   scene.load();

   // React to load completion
   connect(&scene, &DataFlowGraphicsScene::sceneLoaded, [&view]() {
       view.centerScene();
   });

Custom Model Serialization
--------------------------

For custom ``AbstractGraphModel`` subclasses, implement ``saveNode()`` and ``loadNode()``:

.. code-block:: cpp

   QJsonObject MyModel::saveNode(NodeId nodeId) const
   {
       QJsonObject json;

       // Required: ID
       json["id"] = static_cast<qint64>(nodeId);

       // Required: Position
       QPointF pos = nodeData(nodeId, NodeRole::Position).toPointF();
       json["position"] = QJsonObject{{"x", pos.x()}, {"y", pos.y()}};

       // Optional: Your custom data
       json["internal-data"] = QJsonObject{
           {"type", _nodeTypes[nodeId]},
           {"custom-field", _customData[nodeId]}
       };

       return json;
   }

   void MyModel::loadNode(QJsonObject const& json)
   {
       NodeId nodeId = static_cast<NodeId>(json["id"].toInt());

       // Ensure unique IDs
       _nextId = std::max(_nextId, nodeId + 1);

       // Create node
       _nodes.insert(nodeId);
       emit nodeCreated(nodeId);

       // Restore position
       QJsonObject posJson = json["position"].toObject();
       setNodeData(nodeId, NodeRole::Position,
                   QPointF(posJson["x"].toDouble(), posJson["y"].toDouble()));

       // Restore custom data
       QJsonObject internal = json["internal-data"].toObject();
       _nodeTypes[nodeId] = internal["type"].toString();
       _customData[nodeId] = internal["custom-field"].toString();
   }

Then implement full save/load:

.. code-block:: cpp

   QJsonObject MyModel::save() const
   {
       QJsonArray nodesArray;
       for (NodeId nodeId : allNodeIds()) {
           nodesArray.append(saveNode(nodeId));
       }

       QJsonArray connectionsArray;
       for (NodeId nodeId : allNodeIds()) {
           for (auto& conn : allConnectionIds(nodeId)) {
               // Avoid duplicates: only save from output side
               if (conn.outNodeId == nodeId) {
                   connectionsArray.append(toJson(conn));
               }
           }
       }

       return QJsonObject{
           {"nodes", nodesArray},
           {"connections", connectionsArray}
       };
   }

   void MyModel::load(QJsonObject const& json)
   {
       // Clear existing
       for (NodeId id : allNodeIds()) {
           deleteNode(id);
       }

       // Load nodes
       for (auto nodeValue : json["nodes"].toArray()) {
           loadNode(nodeValue.toObject());
       }

       // Load connections
       for (auto connValue : json["connections"].toArray()) {
           ConnectionId conn = fromJson(connValue.toObject());
           addConnection(conn);
       }
   }

NodeDelegateModel Serialization
-------------------------------

Delegates can save custom state:

.. code-block:: cpp

   class MyNode : public NodeDelegateModel
   {
   public:
       QJsonObject save() const override
       {
           QJsonObject json = NodeDelegateModel::save();
           json["my-value"] = _spinBox->value();
           return json;
       }

       void load(QJsonObject const& json) override
       {
           NodeDelegateModel::load(json);
           _spinBox->setValue(json["my-value"].toDouble());
       }

   private:
       QDoubleSpinBox* _spinBox;
   };

Connection ID Utilities
-----------------------

Helper functions in ``ConnectionIdUtils.hpp``:

.. code-block:: cpp

   #include <QtNodes/ConnectionIdUtils>

   // Convert to/from JSON
   QJsonObject json = QtNodes::toJson(connectionId);
   ConnectionId conn = QtNodes::fromJson(json);

Complete Save/Load Example
--------------------------

.. code-block:: cpp

   // In your main window
   QAction* saveAction = fileMenu->addAction("Save", [&]() {
       QString path = QFileDialog::getSaveFileName(
           this, "Save Graph", "", "JSON Files (*.json)");
       if (path.isEmpty()) return;

       QFile file(path);
       if (file.open(QIODevice::WriteOnly)) {
           QJsonDocument doc(model.save());
           file.write(doc.toJson(QJsonDocument::Indented));
       }
   });

   QAction* loadAction = fileMenu->addAction("Load", [&]() {
       QString path = QFileDialog::getOpenFileName(
           this, "Load Graph", "", "JSON Files (*.json)");
       if (path.isEmpty()) return;

       QFile file(path);
       if (file.open(QIODevice::ReadOnly)) {
           QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
           model.load(doc.object());
           view.centerScene();
       }
   });

.. seealso::

   - ``examples/calculator/`` -- Save/load implementation
   - :doc:`undo-redo` -- Undo uses serialization internally
