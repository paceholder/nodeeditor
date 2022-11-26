Porting Code from Version 2.x
=============================


Renamed Classes
---------------

The majority of classes work without significant changes, you might need to
rename some base clasess you are inheriting from.


.. table::
     :widths: 10 10 30

     ==================== ========================== ==============
     Classes in v2        Classes in v3              Comment
     ==================== ========================== ==============
     Node                  --                        The node is represented now just by its
                                                     internal id which is stored in a central
                                                     graph class.

     Connection            --                        Ditto

     NodeDataModel         NodeDelegateModel         In new terms a single model defines
                                                     the whole graph structure. Hence
                                                     smaller per-node models became
                                                     delegates.

     DataModelRegistry     NodeDelegateModelRegistry See comment above

     FlowView              GraphicsView

     FlowScene             DataFlowGraphicsScene     The new class inherits from
                                                     ``BasicGraphicsScene``

      --                   DataFlowGraphModel        This is a new central class
                                                     that defines the whole graph structure.
                                                     The class takes
                                                     ``NodeDelegatNodeDelegateModelRegistry``
                                                     in the constructur and
                                                     populates delegate models
                                                     internally. The graph model
                                                     itself is then passed to the
                                                     ``DataFlowGraphicsScene``.

     NodePainter          DefaultNodePainter         Previously a ``NodePainter``
                                                     was created dynamically on
                                                     the stack right in the
                                                     painting routine. Now
                                                     painter is a class instance
                                                     constanly living in the
                                                     scene. It could be replaced
                                                     by a user-defined clas
                                                     inherited from
                                                     ``AbstractNodePainter``
     ==================== ========================== ==============


Removed Features
----------------


Some minor capabilities were removed in version 3:

- Warning messages at the bottom of the nodes. They were shown when the data was
  incosistent or upon any other error signalized by a node.
  The feature was useful in some cases but wasn't visually appealing and caused a
  node resize/repainting events.
- Data Type Converters. Such classes were registered among Node Data Models and
  made ports of different types compatible. I prefer to leave it up to the
  ``AbstractGraphModel`` derivative to decide what could be attached and what
  not. See the function ``AbstractGraphModel::connectionPossible``.

