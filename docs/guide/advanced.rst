Advanced Topics
===============

This guide covers dynamic ports, node locking, loop detection, and other
advanced features.

Dynamic Ports
-------------

Add or remove ports at runtime. This is useful for nodes with variable
inputs like "Add N Numbers" or "Merge Arrays".

.. image:: /_static/screenshots/dynamic-ports.png
   :alt: Node with dynamically added ports
   :width: 500px

..
   SCREENSHOT NEEDED: dynamic-ports.png
   - Show dynamic_ports example
   - Node with "Add Port" button
   - Multiple dynamically added ports visible
   - Size: ~300px wide

**The process:**

1. Call ``portsAboutToBeInserted()`` or ``portsAboutToBeDeleted()``
2. Modify your internal data
3. Call ``portsInserted()`` or ``portsDeleted()``

.. code-block:: cpp

   void MyModel::addPortToNode(NodeId nodeId)
   {
       // 1. Prepare: library caches affected connections
       portsAboutToBeInserted(nodeId, PortType::In, newPortIndex, newPortIndex);

       // 2. Update your data
       _portCounts[nodeId]++;

       // 3. Complete: library restores shifted connections
       portsInserted();
   }

   void MyModel::removePortFromNode(NodeId nodeId, PortIndex portIndex)
   {
       // 1. Prepare: library removes affected connections
       portsAboutToBeDeleted(nodeId, PortType::In, portIndex, portIndex);

       // 2. Update your data
       _portCounts[nodeId]--;

       // 3. Complete
       portsDeleted();
   }

**In NodeDelegateModel:**

.. code-block:: cpp

   void MyDelegate::addPort()
   {
       emit portsAboutToBeInserted(PortType::In, newIndex, newIndex);

       _inputCount++;

       emit portsInserted();
   }

.. warning::

   Always use the two-phase approach. Modifying ports without the
   ``portsAboutTo...`` / ``ports...`` calls will corrupt connection state.

Locked Nodes
------------

Prevent nodes from being moved or selected:

.. code-block:: cpp

   NodeFlags MyModel::nodeFlags(NodeId nodeId) const override
   {
       NodeFlags flags = AbstractGraphModel::nodeFlags(nodeId);

       if (shouldBeLocked(nodeId)) {
           flags |= NodeFlag::Locked;
       }

       return flags;
   }

Update at runtime:

.. code-block:: cpp

   void MyModel::lockNode(NodeId nodeId)
   {
       _lockedNodes.insert(nodeId);
       emit nodeFlagsUpdated(nodeId);  // Tell scene to update
   }

.. image:: /_static/screenshots/locked-node.png
   :alt: Locked node (grayed out or with lock icon)
   :width: 400px

..
   SCREENSHOT NEEDED: locked-node.png
   - Show a locked node (from lock_nodes_and_connections example)
   - Visually distinct from normal nodes
   - Maybe slightly faded or with indicator
   - Size: ~200px wide

Resizable Nodes
---------------

Allow users to resize nodes (useful for embedded widgets):

.. code-block:: cpp

   NodeFlags MyModel::nodeFlags(NodeId nodeId) const override
   {
       return NodeFlag::Resizable;
   }

Handle size changes:

.. code-block:: cpp

   bool MyModel::setNodeData(NodeId nodeId, NodeRole role, QVariant value) override
   {
       if (role == NodeRole::Size) {
           _nodeSizes[nodeId] = value.toSize();
           emit nodeUpdated(nodeId);
           return true;
       }
       // ...
   }

Non-Detachable Connections
--------------------------

Prevent users from dragging connections away from certain nodes:

.. code-block:: cpp

   bool MyModel::detachPossible(ConnectionId conn) const override
   {
       // Don't allow detaching from "output" nodes
       if (isOutputNode(conn.outNodeId)) {
           return false;
       }
       return true;
   }

Loop Detection
--------------

The ``AbstractGraphModel`` provides ``loopsEnabled()`` to control cyclic connections:

.. code-block:: cpp

   // Default: loops allowed
   bool AbstractGraphModel::loopsEnabled() const { return true; }

   // DataFlowGraphModel: loops disabled
   bool DataFlowGraphModel::loopsEnabled() const override { return false; }

When loops are disabled, ``connectionPossible()`` automatically rejects
connections that would create cycles.

**Custom loop policy:**

.. code-block:: cpp

   class MyModel : public AbstractGraphModel
   {
   public:
       bool loopsEnabled() const override
       {
           return _allowLoops;  // User-configurable
       }

       void setLoopsAllowed(bool allowed)
       {
           _allowLoops = allowed;
       }

   private:
       bool _allowLoops = false;
   };

Connection Policies
-------------------

Control how many connections a port accepts:

.. code-block:: cpp

   QVariant MyModel::portData(NodeId nodeId, PortType portType,
                               PortIndex portIndex, PortRole role) const override
   {
       if (role == PortRole::ConnectionPolicyRole) {
           if (portType == PortType::In) {
               // Inputs accept only one connection
               return QVariant::fromValue(ConnectionPolicy::One);
           } else {
               // Outputs can connect to many inputs
               return QVariant::fromValue(ConnectionPolicy::Many);
           }
       }
       // ...
   }

Custom Connection Validation
----------------------------

Implement complex connection rules:

.. code-block:: cpp

   bool MyModel::connectionPossible(ConnectionId conn) const override
   {
       // Basic checks
       if (!nodeExists(conn.inNodeId) || !nodeExists(conn.outNodeId))
           return false;

       // No self-connections
       if (conn.inNodeId == conn.outNodeId)
           return false;

       // Type compatibility
       auto outType = getOutputType(conn.outNodeId, conn.outPortIndex);
       auto inType = getInputType(conn.inNodeId, conn.inPortIndex);

       if (!typesCompatible(outType, inType))
           return false;

       // Custom rule: max 3 connections to any input
       if (connections(conn.inNodeId, PortType::In, conn.inPortIndex).size() >= 3)
           return false;

       // Cycle detection (if loops disabled)
       if (!loopsEnabled() && wouldCreateCycle(conn))
           return false;

       return true;
   }

Programmatic Graph Manipulation
-------------------------------

Build graphs in code (useful for testing or loading custom formats):

.. code-block:: cpp

   void buildGraph(AbstractGraphModel& model)
   {
       // Create nodes
       NodeId source = model.addNode("Source");
       model.setNodeData(source, NodeRole::Position, QPointF(0, 0));

       NodeId process = model.addNode("Process");
       model.setNodeData(process, NodeRole::Position, QPointF(200, 0));

       NodeId output = model.addNode("Output");
       model.setNodeData(output, NodeRole::Position, QPointF(400, 0));

       // Create connections
       if (model.connectionPossible({source, 0, process, 0})) {
           model.addConnection({source, 0, process, 0});
       }

       if (model.connectionPossible({process, 0, output, 0})) {
           model.addConnection({process, 0, output, 0});
       }
   }

Node Groups (Future)
--------------------

.. note::

   Node grouping is planned but not yet implemented. See the
   `GitHub issues <https://github.com/paceholder/nodeeditor/issues>`_ for status.

.. seealso::

   - ``examples/dynamic_ports/`` -- Dynamic port example
   - ``examples/lock_nodes_and_connections/`` -- Locking example
