Overview
========

Intro
-----


QtNodes is a Qt-based library designed for graphical representation of
the node graphs and performing various operations on them.

.. image:: /_static/calculator.png

The project is built with help of the CMake configurator. Therefore it
is quite easy to incorporate the library into any CMake-based Qt
project.

As of version `3.0` the library uses the Model-View approach. The
central class :cpp:type:`AbstractGraphModel` is a starting point for user graph
models. It wraps the data representing the graph and forwards it
to the `BasicGraphicsScene` -- a class responsible for populating
`QGraphicsObject` items and showing them on the `QGraphicsView` widget.

The library could be used for two purposes:

  1. General-purpose graph visulalization and editing.
  2. Computing data in the nodes and propagating it through connections.

The "headless" mode is also supported. It is possible to create, delete, connect
and disconnect nodes, as well as propagate data, without assigning your
:cpp:type:`AbstractGraphModel` derivative to a :cpp:type:`BasicGraphicsScene`.

Examples Directory Layout
-------------------------

The examples could be found in the directory ``examples``:

- ``graph``. Demonstrates usage of AbstractGraphModel for general
  graph visulalization and editing.
- ``dynamic_ports``. Shows what needs to be done to dynamically create and
  destroy node ports.
- ``lock_nodes_and_connections``. Demonstrates two capabilities of
  "non-detachable" connectinos and "locked" nodes (non-movable, non-selectable).
- legacy "data flow" examples from versions prior to ``3.0``:
  - ``text``. Text is propagated between the nodes.
  - ``calculator/main.cpp``. Dataflow-based implementation of the simplest
    calculator. We use an advanced model :cpp:type:`QtNodes::DataFlowGraphModel`
    capable of storing the registry of NodeDataModel and propagating user data
    beween the nodes.
  - ``calculator/headless_main.cpp``. The example loads a scene saved by a
    GUI-based ``calculator`` example and computes several results without
    creating GUI elements.
  - ``connection_colors``. Demonstrates the ability to color the
    connections in correspondence to the connected data types.
  - ``resizable_images``. The examples shows how to embed a widget into nodes and
    how to make the nodes resizable.
  - ``styles``. The example demonstrates graph style customization.



Feedback Wanted
---------------

Make a request on `Github <https://github.com/paceholder/nodeeditor>`_ if
something is unclear in the code or in the documentation.
