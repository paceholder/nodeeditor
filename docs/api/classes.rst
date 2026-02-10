QtNodes Class Reference
=======================

This page provides auto-generated API documentation from the source code.

Core Classes
------------

Graph Model
^^^^^^^^^^^

.. doxygenclass:: QtNodes::AbstractGraphModel
   :members:

.. doxygenclass:: QtNodes::DataFlowGraphModel
   :members:

Scene and View
^^^^^^^^^^^^^^

.. doxygenclass:: QtNodes::BasicGraphicsScene
   :members:

.. doxygenclass:: QtNodes::DataFlowGraphicsScene
   :members:

.. doxygenclass:: QtNodes::GraphicsView
   :members:

Node Classes
------------

Graphics
^^^^^^^^

.. doxygenclass:: QtNodes::NodeGraphicsObject
   :members:
   :no-link:

.. doxygenclass:: QtNodes::NodeState
   :members:

Painting
^^^^^^^^

.. doxygenclass:: QtNodes::AbstractNodePainter
   :members:

.. doxygenclass:: QtNodes::DefaultNodePainter
   :members:

Geometry
^^^^^^^^

.. doxygenclass:: QtNodes::AbstractNodeGeometry
   :members:

.. doxygenclass:: QtNodes::DefaultHorizontalNodeGeometry
   :members:

.. doxygenclass:: QtNodes::DefaultVerticalNodeGeometry
   :members:

Connection Classes
------------------

.. doxygenclass:: QtNodes::ConnectionGraphicsObject
   :members:
   :no-link:

.. doxygenclass:: QtNodes::AbstractConnectionPainter
   :members:

.. doxygenclass:: QtNodes::DefaultConnectionPainter
   :members:

.. doxygenclass:: QtNodes::NodeConnectionInteraction
   :members:

Data Flow Classes
-----------------

.. doxygenclass:: QtNodes::NodeDelegateModel
   :members:

.. doxygenclass:: QtNodes::NodeDelegateModelRegistry
   :members:

.. doxygenclass:: QtNodes::NodeData
   :members:

Styling
-------

.. doxygenclass:: QtNodes::NodeStyle
   :members:

.. doxygenclass:: QtNodes::ConnectionStyle
   :members:

.. doxygenclass:: QtNodes::GraphicsViewStyle
   :members:

.. doxygenclass:: QtNodes::StyleCollection
   :members:

Undo Commands
-------------

.. doxygenclass:: QtNodes::CreateCommand
   :members:

.. doxygenclass:: QtNodes::DeleteCommand
   :members:

.. doxygenclass:: QtNodes::ConnectCommand
   :members:

.. doxygenclass:: QtNodes::DisconnectCommand
   :members:

.. doxygenclass:: QtNodes::MoveNodeCommand
   :members:

.. doxygenclass:: QtNodes::CopyCommand
   :members:

.. doxygenclass:: QtNodes::PasteCommand
   :members:

Data Types
----------

Structs
^^^^^^^

.. doxygenstruct:: QtNodes::NodeDataType
   :members:

.. doxygenstruct:: QtNodes::ConnectionId
   :members:

.. doxygenstruct:: QtNodes::NodeValidationState
   :members:

.. doxygenstruct:: QtNodes::ProcessingIconStyle
   :members:

Type Definitions
^^^^^^^^^^^^^^^^

.. doxygentypedef:: QtNodes::NodeId

.. doxygentypedef:: QtNodes::PortIndex

Enumerations
^^^^^^^^^^^^

.. doxygenenum:: QtNodes::NodeRole

.. doxygenenum:: QtNodes::NodeFlag

.. doxygenenum:: QtNodes::PortRole

.. doxygenenum:: QtNodes::PortType

.. doxygenenum:: QtNodes::ConnectionPolicy

.. doxygenenum:: QtNodes::NodeProcessingStatus

.. doxygenenum:: QtNodes::ProcessingIconPos
