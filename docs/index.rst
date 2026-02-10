QtNodes
=======

**A Qt-based library for building node graph editors**

.. image:: /_static/screenshots/calculator.png
   :alt: QtNodes editor showing a calculator example
   :align: center

QtNodes lets you create interactive node-based interfaces for visual programming,
data flow applications, shader editors, state machines, and more. Built on Qt's
Model-View architecture, it separates your data from its visual representation.

.. code-block:: cpp

   // Create a graph and display it in 5 lines
   DataFlowGraphModel model(registry);
   DataFlowGraphicsScene scene(model);
   GraphicsView view(&scene);
   view.show();

Key Features
------------

- **Model-View Architecture** -- Your graph data stays independent of the UI
- **Data Flow Support** -- Built-in data propagation between connected nodes
- **Headless Mode** -- Run graph computations without any GUI
- **Customizable** -- Custom painters, styles, and node geometries
- **Serialization** -- Save and load graphs as JSON
- **Undo/Redo** -- Built-in support via Qt's undo framework

Two Approaches
--------------

.. list-table::
   :widths: 50 50
   :header-rows: 1

   * - Simple Graph Visualization
     - Data Flow Processing
   * - Use ``AbstractGraphModel`` + ``BasicGraphicsScene``
     - Use ``DataFlowGraphModel`` + ``DataFlowGraphicsScene``
   * - You manage all graph data
     - Library manages node delegates and data routing
   * - Best for: visualization, custom graph logic
     - Best for: visual programming, calculators, pipelines

Getting Started
---------------

.. toctree::
   :maxdepth: 1

   getting-started/installation
   getting-started/quickstart
   getting-started/concepts

User Guide
----------

.. toctree::
   :maxdepth: 1

   guide/graph-models
   guide/data-flow
   guide/visualization
   guide/styling
   guide/serialization
   guide/undo-redo
   guide/advanced

Examples
--------

.. toctree::
   :maxdepth: 1

   examples/index

API Reference
-------------

.. toctree::
   :maxdepth: 1

   api/index
   api/classes

Other Resources
---------------

.. toctree::
   :maxdepth: 1

   migration/v2-to-v3
   faq
   testing
   license_link

Indices
-------

* :ref:`genindex`
