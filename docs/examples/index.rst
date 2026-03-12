Examples
========

Complete working examples demonstrating QtNodes features.

.. tip::

   All examples are in the ``examples/`` directory. Build with
   ``-DBUILD_EXAMPLES=ON`` (default).

Gallery
-------

.. list-table::
   :widths: 30 70
   :header-rows: 0

   * - .. image:: /_static/screenshots/calculator.png
          :width: 250px
     - **Calculator** (``examples/calculator/``)

       Full data flow application with number sources, operators, and display.
       Demonstrates embedded widgets, save/load, and menu integration.

       :doc:`/guide/data-flow`

   * - .. image:: /_static/screenshots/flow.png
          :width: 250px
     - **Simple Graph Model** (``examples/simple_graph_model/``)

       Minimal custom graph model implementation. Shows how to subclass
       ``AbstractGraphModel`` and use ``BasicGraphicsScene``.

       :doc:`/guide/graph-models`

   * - .. image:: /_static/screenshots/style-example.png
          :width: 250px
     - **Styles** (``examples/styles/``)

       Custom styling with different color schemes and visual effects.

       :doc:`/guide/styling`

   * - .. image:: /_static/screenshots/connection-colors.png
          :width: 250px
     - **Connection Colors** (``examples/connection_colors/``)

       Data-type-based connection coloring. Each data type has its own color.

       :doc:`/guide/styling`

   * - .. image:: /_static/screenshots/vertical-layout.png
          :width: 250px
     - **Vertical Layout** (``examples/vertical_layout/``)

       Top-to-bottom node arrangement with ports on top and bottom edges.

       :doc:`/guide/visualization`

   * - .. image:: /_static/screenshots/dynamic-ports.png
          :width: 250px
     - **Dynamic Ports** (``examples/dynamic_ports/``)

       Add and remove ports at runtime. Shows the two-phase port modification API.

       :doc:`/guide/advanced`

   * - .. image:: /_static/screenshots/resizable.png
          :width: 250px
     - **Resizable Images** (``examples/resizable_images/``)

       Nodes with embedded image widgets that can be resized by dragging.

       :doc:`/guide/advanced`

   * - .. image:: /_static/screenshots/locked-node.png
          :width: 250px
     - **Lock Nodes & Connections** (``examples/lock_nodes_and_connections/``)

       Prevent node movement and connection detachment.

       :doc:`/guide/advanced`

   * - .. image:: /_static/screenshots/validation.png
          :width: 250px
     - **Node Validation** (``examples/node_validation/``)

       Demonstrates ``NodeValidationState`` and ``NodeProcessingStatus``.
       Shows error/warning states and processing indicators.

       :doc:`/guide/data-flow`

   * - .. image:: /_static/screenshots/custom-painter.png
          :width: 250px
     - **Custom Painter** (``examples/custom_painter/``)

       Custom node and connection rendering with gradients and arrows.

       :doc:`/guide/visualization`

   * - .. image:: /_static/screenshots/text.png
          :width: 250px
     - **Text** (``examples/text/``)

       Simple text propagation between nodes. Good starting point.

       :doc:`/guide/data-flow`

Headless Example
----------------

``examples/calculator/headless_main.cpp`` demonstrates running computations
without any GUI:

.. code-block:: cpp

   // Load a saved graph
   DataFlowGraphModel model(registry);
   model.load(loadJsonFromFile("saved_graph.json"));

   // Modify inputs programmatically
   auto* sourceNode = model.delegateModel<NumberSourceNode>(sourceNodeId);
   sourceNode->setValue(42.0);

   // Read computed output
   auto* displayNode = model.delegateModel<DisplayNode>(displayNodeId);
   double result = displayNode->getValue();

Running Examples
----------------

After building:

.. code-block:: bash

   # From build directory
   ./bin/calculator
   ./bin/simple_graph_model
   ./bin/styles
   # ... etc

Each example is a standalone executable demonstrating specific features.
