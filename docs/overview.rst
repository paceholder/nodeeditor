Overview
========

Intro
-----


QtNodes is a Qt5-based library designed for graphical representation of
the node graphs and performing various operations on them.

.. image:: /_static/calculator.png

The project is built with help of the CMake configurator. Therefore it
is quite easy to incorporate the library into any CMake-based Qt
project.

As of version `3.0` the library uses the Model-View approach. The
central class :cpp:type:`GraphModel` is a starting point for user graph
models. It wraps the data representing the graph and forwards it
to the `BasicGraphicsScene` -- a class responsible for populating
`QGraphicsObject` items and showing them on the `QGraphicsView` widget.

The library could be used for two purposes:

  1. General-purpose graph visulalization and editing.
  2. Computing data in the nodes and propagating it through connections.

The "headless" mode is also supported. It is possible to create,
delete, connect and disconnect nodes, as well as propagate data,
without assigning your :cpp:type:`GraphModel` to a
:cpp:type:`BasicGraphicsScene`.

Examples
--------

The examples could be found in the folder ``examples``:

- ``graph``. Demonstrates usage of AbstractGraphModel for general
  graph visulalization and editing.
- "legacy" examples from versions prior to ``3.0``:

  - ``text``. Text is propagated between the nodes.
  - ``calculator``. Dataflow-based implementation of the simplest
    calculator. We use an advanced model
    :cpp:type:`QtNodes::DataFlowGraphModel` capable of storing the registry of
    NodeDataModel and propagating user data beween the nodes.
  - ``connection_colors``. Demonstrates the ability to color the
    connections in correspondence to the connected data types.
  - ``images``. Another dataflow example where images are propagated
    between the nodes.
  - ``styles``. The example demonstrates graph style customization.


TODOs
-----

1. Dynamic ports
2.  `NodeGeometryDelegate`, `NodePaintDelegate`
3. `ConnectionPaintDelegate`
4. Python Wrapper using Shiboken
5. Python examples
6. Unit-Tests
7. Documentation
8. QML front-end


Node Geometry
-------------

.. code-block::

                         vertical spacing
                        /
            port width                      port width
           |          | |                | |         |

    0 _     _________________________________________    ___
           /                 Caption                 \
           |             ________________            |   ___  caption height
           |             |               |           |
           O In Name     |               |  Out Name O        entry
           |             |               |           |   ___
           |             |               |           |   ___  vertical spacing
           |             |               |           |
           O Another In  |               |  Out Name O
           |             |               |           |
           |             |               |           |
           |             |               |           |
           O             |               |           O
           |             |               |           |
           |             |_______________|           |
           |                                         |
           O                                         |
           |                                         |
           \_________________________________________/




Size is recalculated when
^^^^^^^^^^^^^^^^^^^^^^^^^

0. After construction.
1. Embedding the widget.
2. After resizing.
3. Before painting (conditional, depends on whether the font metrics
   was changed).
4. When incoming data changed (could trigger size changes, maybe in
   captions).
5. When embedded widget changes its size.


