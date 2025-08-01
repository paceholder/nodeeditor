QtNodes
#######

https://github.com/paceholder/nodeeditor/actions/workflows/cmake_build.yml/badge.svg

Introduction
============

**QtNodes** is conceived as a general-purpose Qt-based library aimed at
developing Node Editors for various applications. The library could be used for
simple graph visualization and editing or extended further for using the
`Dataflow paradigm <https://en.wikipedia.org/wiki/Dataflow_programming>`_ .

The library is written using the Model-View approach. The whole graph structure
is defined by a class derived from ``AbstractGraphModel``.  It is possible to
create or add Nodes and Connections. The underlying data structures could be of
any arbitrary type or representation.

An instance of ``AbstractGraphModel`` could or could not be attached to
specialized ``QGraphicsScene`` and ``QGraphicsView`` objects. I.e. the so-called
"headless" `modus operandi` is possible.

Documentation
=============

`Read the Docs for QtNodes <https://qtnodes.readthedocs.io/>`_

Warning
  Many classes were changed in the version ``3.0``. If you had a large project
  based on ``2.x.x``, make sure you read the documentation first and see the
  examples before checking out the new code.

Branches
--------

There are branchses ``v2`` and ``v3`` for versions ``2.x.x`` and ``3.x``
respectively. The branch ``master`` contains the latest dev state.


.. contents:: Navigation
    :depth: 2


Data Flow Paradigm
==================

The extended model class ``DataFlowGraphModel`` allows to register "processing
algorithms" represented by nodes and is equipped with a set of Qt's signals and
slots for propagating the data though the nodes.

The node's algorithm is triggered upon arriving of any new input data. The
computed result is propagated to the output connections. Each new connection
fetches available data and propagates is further. Each change in the source node
is immediately propagated through all the connections updating the whole graph.


Supported Environments
======================

Platforms
---------

* Linux (x64, gcc, Qt 5.15.2) |ImageLink|
* OSX (Apple Clang, Qt 5.15.2) |ImageLink|
* Windows (MSVC, Qt 5.15.2) |ImageLink|
* Windows (MSVC, Qt 6.3.0) |ImageLink|

.. |ImageLink| image:: https://github.com/paceholder/nodeeditor/actions/workflows/cmake_build.yml/badge.svg
   :target: https://github.com/paceholder/nodeeditor/actions/workflows/cmake_build.yml


Dependencies
------------

* Qt >5.15
* CMake 3.8
* Catch2


Current State (v3)
==================

* Model-based graph
* Headless mode
  You can create, populate, modify the derivative of ``AbstractGraphModel``
  without adding it to the actual Flow Scene.  The library is now designed to be
  general-purpose graph visualization and modification tool, without
  specialization on only data propagation.
* Automatic data propagation built on top of the graph-model code
  The library could be used for both pure graph visualization purposes and for
  originally implemented data propagation.
* Dynamic ports
* Datatype-aware connections
* Embedded Qt widgets
* One-output to many-input connections
* JSON-based interface styles
* Saving scenes to JSON files
* Custom Node Geometry
* Vertical and Horizontal graph layouts
* Undo/Redo, Duplication (CTRL+D)


Building
========

Set this option to false if you want to build with Qt5 version instead of Qt6:

::

   USE_QT6

   mkdir build && cd build && cmake .. -DUSE_QT6=on
   or
   mkdir build && cd build && cmake .. -DUSE_QT6=off

For building a static lib use:

::

    cmake .. -BUILD_SHARED_LIBS=off

Linux
-----

::

  git clone git@github.com:paceholder/nodeeditor.git
  cd nodeeditor
  mkdir build
  cd build
  cmake ..
  make -j && make install


Qt Creator
----------

1. Open `CMakeLists.txt` as project.
2. `Build -> Run CMake`
3. `Build -> Build All`
4. Click the button `Run`

.. note::
   The project includes unit tests built with Catch2. If you don't have Catch2 installed, 
   you can disable testing by setting `-DBUILD_TESTING=OFF` in CMake configuration.


With Cmake using `vcpkg`
^^^^^^^^^^^^^^^^^^^^^^^^

1. Install `vcpkg`
2. Add the following flag in configuration step of `CMake`

::

   -DCMAKE_TOOLCHAIN_FILE=<vcpkg_dir>/scripts/buildsystems/scripts/buildsystems/vcpkg.cmake


Testing
=======

QtNodes includes a comprehensive unit test suite built with Catch2.

**Running Tests**

From the build directory:

::

    # Build tests
    make test_nodes
    
    # Run all tests
    ./bin/test_nodes
    
    # Run specific categories
    ./bin/test_nodes "[core]"        # Core functionality tests
    ./bin/test_nodes "[graphics]"    # Graphics system tests

**Test Coverage**

* Core model operations (node CRUD, connections)
* Signal emission verification (AbstractGraphModel signals)
* Serialization (JSON save/load) 
* Undo system integration
* Graphics scene management
* Connection utilities

For detailed testing documentation, see the `Testing Guide <https://qtnodes.readthedocs.io/en/latest/testing.html>`_.


Help Needed
===========

#. Python wrappring using PySide.
#. QML frontend.
#. Wirting a ClangFormat config.

Any suggestions are welcome!


Contribution
============

#. Be polite, respectful and collaborative.
#. For submitting a bug:

   #. Describe your environment (Qt version, compiler, OS etc)
   #. Describe steps to reproduce the issue

#. For submitting a pull request:

   #. Create a proposal task first. We can come up with a better design together.
   #. Create a pull-request. If applicable, create a simple example for your
      problem, describe the changes in details, provide use cases.

#. For submitting a development request:

   #. Describe your issue in details
   #. Provide some use cases.

#. I maintain this probject in my free time, when I am not busy with my work or
   my family. **If I do not react or do not answer for too long, please ping
   me**.


Citing
======

::

    Dmitry Pinaev et al, Qt Nodes, (2013-2025), GitHub repository, https://github.com/paceholder/nodeeditor

BibTeX::

    @misc{Pinaev2025,
      author = {Dmitry Pinaev et al},
      title = {QtNodes. Node Editor},
      year = {2025},
      publisher = {GitHub},
      journal = {GitHub repository},
      howpublished = {\url{https://github.com/paceholder/nodeeditor}},
      commit = {877ddb8c447a7a061a5022e9956a3194132e3dd9}
    }

Support
=======

If you like the project you could donate me on PayPal |ImagePaypal|_

.. |ImagePaypal| image:: https://img.shields.io/badge/Donate-PayPal-green.svg
.. _ImagePaypal: https://www.paypal.com/paypalme/DmitryPinaev


If you send more than $100, I'll forward $100 to some fund supporting sick
children and report to you back.


Thanks
======

The version 3 was released with a generous help of
`Davide Faconti <https://github.com/facontidavide>`_


Showcase
========

Youtube videos
--------------

.. image:: https://img.youtube.com/vi/pxMXjSvlOFw/0.jpg
   :target: https://www.youtube.com/watch?v=pxMXjSvlOFw

|

.. image:: https://img.youtube.com/vi/i_pB-Y0hCYQ/0.jpg
   :target: https://www.youtube.com/watch?v=i_pB-Y0hCYQ

CANdevStudio
------------

`CANdevStudio <https://github.com/GENIVI/CANdevStudio>`_ is a cost-effective,
cross-platform replacement for CAN simulation software. CANdevStudio enables to
simulate CAN signals such as ignition status, doors status or reverse gear by
every automotive developer. Thanks to modularity it is easy to implement new,
custom features.


.. image:: docs/_static/showcase_CANdevStudio.png


Chigraph
--------

`Chigraph <https://github.com/chigraph/chigraph>`_ is a visual programming
language for beginners that is unique in that it is an intuitive flow graph:

.. image:: docs/_static/chigraph.png


It features easy bindings to C/C++, package management, and a cool interface.


Spkgen particle editor
----------------------

`Spkgen <https://github.com/fredakilla/spkgen>`_ is an editor for the SPARK
particles engine that uses a node-based interface to create particles effects for
games

.. image:: docs/_static/spkgen.png
