API Overview
============

This section provides API reference documentation for QtNodes.

Quick Reference
---------------

**Core Classes**

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Class
     - Purpose
   * - ``AbstractGraphModel``
     - Base class for all graph models
   * - ``DataFlowGraphModel``
     - Model with automatic data propagation
   * - ``BasicGraphicsScene``
     - Visualizes any graph model
   * - ``DataFlowGraphicsScene``
     - Scene for data flow models (with menus)
   * - ``GraphicsView``
     - View widget with interactions
   * - ``NodeDelegateModel``
     - Base class for node logic in data flow

**Graphics Classes**

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Class
     - Purpose
   * - ``NodeGraphicsObject``
     - Visual representation of a node
   * - ``ConnectionGraphicsObject``
     - Visual representation of a connection
   * - ``AbstractNodePainter``
     - Interface for custom node rendering
   * - ``AbstractConnectionPainter``
     - Interface for custom connection rendering
   * - ``AbstractNodeGeometry``
     - Interface for custom node layout

**Style Classes**

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Class
     - Purpose
   * - ``StyleCollection``
     - Global style management
   * - ``NodeStyle``
     - Node appearance settings
   * - ``ConnectionStyle``
     - Connection appearance settings
   * - ``GraphicsViewStyle``
     - Canvas appearance settings

**Data Types**

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Type
     - Purpose
   * - ``NodeId``
     - ``unsigned int`` - unique node identifier
   * - ``PortIndex``
     - ``unsigned int`` - port number (0-based)
   * - ``ConnectionId``
     - Struct identifying a connection
   * - ``NodeRole``
     - Enum for node data queries
   * - ``PortRole``
     - Enum for port data queries
   * - ``PortType``
     - ``In``, ``Out``, or ``None``
   * - ``ConnectionPolicy``
     - ``One`` or ``Many`` connections

**Enums**

.. list-table::
   :widths: 30 70
   :header-rows: 1

   * - Enum
     - Values
   * - ``NodeRole``
     - Type, Position, Size, Caption, CaptionVisible, Style, InternalData,
       InPortCount, OutPortCount, Widget, ValidationState, ProcessingStatus
   * - ``PortRole``
     - Data, DataType, ConnectionPolicyRole, CaptionVisible, Caption
   * - ``NodeFlag``
     - NoFlags, Resizable, Locked
   * - ``NodeValidationState::State``
     - Valid, Warning, Error
   * - ``NodeProcessingStatus``
     - NoStatus, Updated, Processing, Pending, Empty, Failed, Partial

Header Files
------------

All public headers are in ``QtNodes/``:

.. code-block:: cpp

   // Core
   #include <QtNodes/AbstractGraphModel>
   #include <QtNodes/DataFlowGraphModel>
   #include <QtNodes/NodeDelegateModel>
   #include <QtNodes/NodeDelegateModelRegistry>
   #include <QtNodes/NodeData>

   // Graphics
   #include <QtNodes/BasicGraphicsScene>
   #include <QtNodes/DataFlowGraphicsScene>
   #include <QtNodes/GraphicsView>

   // Styling
   #include <QtNodes/StyleCollection>
   #include <QtNodes/NodeStyle>
   #include <QtNodes/ConnectionStyle>
   #include <QtNodes/GraphicsViewStyle>

   // Utilities
   #include <QtNodes/Definitions>
   #include <QtNodes/ConnectionIdUtils>

Full API Reference
------------------

See :doc:`classes` for complete Doxygen-generated API documentation.

.. toctree::
   :hidden:

   classes
