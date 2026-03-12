Graph Models
============

The graph model is the core of your application. It stores nodes, connections,
and all associated data. This guide covers implementing your own model by
subclassing ``AbstractGraphModel``.

.. tip::

   If you want automatic data propagation between nodes, see :doc:`data-flow`
   instead. Use a custom graph model when you need full control over graph logic.

AbstractGraphModel Overview
---------------------------

Your model must implement these pure virtual methods:

.. code-block:: cpp

   class MyGraphModel : public QtNodes::AbstractGraphModel
   {
   public:
       // ID generation
       NodeId newNodeId() override;

       // Node queries
       std::unordered_set<NodeId> allNodeIds() const override;
       bool nodeExists(NodeId) const override;
       QVariant nodeData(NodeId, NodeRole) const override;
       bool setNodeData(NodeId, NodeRole, QVariant) override;

       // Connection queries
       std::unordered_set<ConnectionId> allConnectionIds(NodeId) const override;
       std::unordered_set<ConnectionId> connections(NodeId, PortType, PortIndex) const override;
       bool connectionExists(ConnectionId) const override;
       bool connectionPossible(ConnectionId) const override;

       // Mutations
       NodeId addNode(QString nodeType) override;
       void addConnection(ConnectionId) override;
       bool deleteNode(NodeId) override;
       bool deleteConnection(ConnectionId) override;

       // Port queries
       QVariant portData(NodeId, PortType, PortIndex, PortRole) const override;
       bool setPortData(NodeId, PortType, PortIndex, QVariant, PortRole) override;
   };

Implementing Node Management
----------------------------

**ID Generation**

Generate unique IDs. A simple counter works:

.. code-block:: cpp

   NodeId MyGraphModel::newNodeId()
   {
       return _nextId++;
   }

**Adding Nodes**

Store the node and emit the signal:

.. code-block:: cpp

   NodeId MyGraphModel::addNode(QString nodeType)
   {
       NodeId id = newNodeId();
       _nodes.insert(id);
       _nodeTypes[id] = nodeType;
       _nodePositions[id] = QPointF(0, 0);

       emit nodeCreated(id);  // Required!
       return id;
   }

**Deleting Nodes**

Remove connections first, then the node:

.. code-block:: cpp

   bool MyGraphModel::deleteNode(NodeId nodeId)
   {
       if (!nodeExists(nodeId))
           return false;

       // Remove all connections involving this node
       for (auto& conn : allConnectionIds(nodeId)) {
           deleteConnection(conn);
       }

       _nodes.erase(nodeId);
       emit nodeDeleted(nodeId);  // Required!
       return true;
   }

NodeRole Reference
------------------

Implement ``nodeData()`` and ``setNodeData()`` to provide node information:

.. list-table::
   :widths: 20 15 65
   :header-rows: 1

   * - Role
     - Type
     - Description
   * - ``Type``
     - ``QString``
     - Node type identifier (e.g., "AddNode", "ImageFilter")
   * - ``Position``
     - ``QPointF``
     - Position on the canvas
   * - ``Size``
     - ``QSize``
     - Size hint for embedded widgets
   * - ``Caption``
     - ``QString``
     - Display name shown on the node
   * - ``CaptionVisible``
     - ``bool``
     - Whether to show the caption
   * - ``Style``
     - ``QVariantMap``
     - Per-node style overrides (JSON)
   * - ``InternalData``
     - ``QVariantMap``
     - Custom data for serialization
   * - ``InPortCount``
     - ``unsigned int``
     - Number of input ports
   * - ``OutPortCount``
     - ``unsigned int``
     - Number of output ports
   * - ``Widget``
     - ``QWidget*``
     - Embedded widget (or nullptr)
   * - ``ValidationState``
     - ``NodeValidationState``
     - Current validation state
   * - ``ProcessingStatus``
     - ``NodeProcessingStatus``
     - Current processing status

**Example implementation:**

.. code-block:: cpp

   QVariant MyGraphModel::nodeData(NodeId nodeId, NodeRole role) const
   {
       switch (role) {
       case NodeRole::Type:
           return _nodeTypes.value(nodeId);

       case NodeRole::Position:
           return _nodePositions.value(nodeId);

       case NodeRole::Caption:
           return QString("Node %1").arg(nodeId);

       case NodeRole::InPortCount:
           return 2u;  // All nodes have 2 inputs

       case NodeRole::OutPortCount:
           return 1u;  // All nodes have 1 output

       default:
           return {};
       }
   }

Implementing Connections
------------------------

**Connection Queries**

Return connections filtered by node and port:

.. code-block:: cpp

   std::unordered_set<ConnectionId>
   MyGraphModel::connections(NodeId nodeId, PortType portType, PortIndex portIndex) const
   {
       std::unordered_set<ConnectionId> result;
       for (const auto& conn : _connections) {
           if (portType == PortType::In &&
               conn.inNodeId == nodeId &&
               conn.inPortIndex == portIndex) {
               result.insert(conn);
           }
           else if (portType == PortType::Out &&
                    conn.outNodeId == nodeId &&
                    conn.outPortIndex == portIndex) {
               result.insert(conn);
           }
       }
       return result;
   }

**Connection Validation**

Control what connections are allowed:

.. code-block:: cpp

   bool MyGraphModel::connectionPossible(ConnectionId conn) const
   {
       // Nodes must exist
       if (!nodeExists(conn.inNodeId) || !nodeExists(conn.outNodeId))
           return false;

       // No self-connections
       if (conn.inNodeId == conn.outNodeId)
           return false;

       // No duplicate connections
       if (connectionExists(conn))
           return false;

       // Custom logic: check port compatibility, etc.
       return true;
   }

PortRole Reference
------------------

Implement ``portData()`` for port-specific information:

.. list-table::
   :widths: 25 15 60
   :header-rows: 1

   * - Role
     - Type
     - Description
   * - ``Data``
     - ``std::shared_ptr<NodeData>``
     - The actual data at this port
   * - ``DataType``
     - ``NodeDataType``
     - Type descriptor for compatibility checks
   * - ``ConnectionPolicyRole``
     - ``ConnectionPolicy``
     - ``One`` (single connection) or ``Many``
   * - ``Caption``
     - ``QString``
     - Port label text
   * - ``CaptionVisible``
     - ``bool``
     - Whether to show the label

Required Signals
----------------

Your model **must** emit these signals at the appropriate times:

.. code-block:: cpp

   // After adding a node
   emit nodeCreated(nodeId);

   // After removing a node
   emit nodeDeleted(nodeId);

   // After node data changes (caption, style, etc.)
   emit nodeUpdated(nodeId);

   // After position changes specifically
   emit nodePositionUpdated(nodeId);

   // After adding a connection
   emit connectionCreated(connectionId);

   // After removing a connection
   emit connectionDeleted(connectionId);

.. warning::

   Forgetting to emit signals will cause the view to become out of sync
   with your model.

Serialization Support
---------------------

Override ``saveNode()`` and ``loadNode()`` to support save/load:

.. code-block:: cpp

   QJsonObject MyGraphModel::saveNode(NodeId nodeId) const
   {
       QJsonObject json;
       json["id"] = static_cast<int>(nodeId);
       json["type"] = _nodeTypes[nodeId];

       QJsonObject pos;
       pos["x"] = _nodePositions[nodeId].x();
       pos["y"] = _nodePositions[nodeId].y();
       json["position"] = pos;

       return json;
   }

See :doc:`serialization` for complete save/load implementation.

Complete Example
----------------

See ``examples/simple_graph_model/`` for a complete, working implementation
of a custom graph model.

.. seealso::

   - :doc:`data-flow` -- For automatic data propagation
   - :doc:`/api/classes` -- Full API reference
