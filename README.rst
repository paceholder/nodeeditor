QtNodes
#######

Introduction
============

QtNodes is conceived as a general-purpose Qt-based library aimed at developing Node Editors for various applications. The library could be used for simple graph visualization and editing or extended further for using the Dataflow paradigm.

The library is written using the Model-View approach. The whole graph structure is defined by the user-provided ``GraphModel`` class. It is possible to create or add Nodes and Connections. The underlying data structures could be of any arbitrary type or representation.

The ``GraphModel`` could or could not be attached to specialized ``QGraphicsScene`` and ``QGraphicsView`` objects. I.e. the so-called "headless" `modus operandi` is possible.


Data Flow Paradigm
==================

The extended model class ``DataFlowGraphModel`` allows to register "processing algorithms" represented by nodes and is equipped with a set of Qt's signals and slots for propagating the data though the nodes.

The node's algorithm is triggered upon arriving of any new input data. The computed result is propagated to the output connections. Each new connection fetches available data and propagates is further. Each change in the source node is immediately propagated through all the connections updating the whole graph.

Supported Platforms
===================



* Linux (x64, gcc-7.0, clang-7) |ImageLink|_
* OSX (Apple Clang - LLVM 3.6) |ImageLink|_

.. |ImageLink| image:: https://travis-ci.org/paceholder/nodeeditor.svg?branch=master
.. _ImageLink: https://travis-ci.org/paceholder/nodeeditor


* Windows (Win32, x64, msvc2017, MinGW 5.3) |AppveyorImage|_

.. |AppveyorImage| image:: https://ci.appveyor.com/api/projects/status/wxp47wv3uyyiujjw/branch/master?svg=true
.. _AppveyorImage: https://ci.appveyor.com/project/paceholder/nodeeditor/branch/master)

Dependencies
============

* Qt >5.2
* CMake 3.2
* Catch2


Building
========

Linux
^^^^^

::

  git clone git@github.com:paceholder/nodeeditor.git
  cd nodeeditor
  mkdir build
  cd build
  cmake ..
  make -j && make install


Qt Creator
^^^^^^^^^^

1. Open `CMakeLists.txt` as project.
2. If you don't have the `Catch2` library installed, go to `Build Settings`, disable the checkbox `BUILD_TESTING`.
3. `Build -> Run CMake`
4. `Build -> Build All`
5. Click the button `Run`


Roadmap
=======


1. Extend Unit-Tests                                                           
2. Python Wrapper using Shiboken                                        
3. Python examples                                                      
4. QML front-end                                                        
5. NodePaintDelegate & NodeGeometryDelegate
6. ConnectionPaintDelegate                                              
7. Documentation & Tutorial
8. Implement grouping nodes

Citing
======

::

    Dmitry Pinaev et al, Qt5 Node Editor, (2017), GitHub repository, https://github.com/paceholder/nodeeditor

BibTeX::

    @misc{Pinaev2017,
      author = {Dmitry Pinaev et al},
      title = {QtNodes. Node Editor},
      year = {2017},
      publisher = {GitHub},
      journal = {GitHub repository},
      howpublished = {\url{https://github.com/paceholder/nodeeditor}},
      commit = {1d1757d09b03cea0e4921bc19659465fe6e65b9b}
    }

Support
=======

If you like the project you could donate me on PayPal |ImagePaypal|_

.. |ImagePaypal| image:: https://img.shields.io/badge/Donate-PayPal-green.svg
.. _ImagePaypal: https://www.paypal.com/paypalme/DmitryPinaev


If you send more than $10, I'll forward the money to some fund supporting sick children and report to you back.


Showcase
========

Youtube videos
^^^^^^^^^^^^^^

.. image:: https://img.youtube.com/vi/pxMXjSvlOFw/0.jpg
   :target: https://www.youtube.com/watch?v=pxMXjSvlOFw

|

.. image:: https://img.youtube.com/vi/i_pB-Y0hCYQ/0.jpg
   :target: https://www.youtube.com/watch?v=i_pB-Y0hCYQ


Chigraph
^^^^^^^^

`Chigraph <https://github.com/chigraph/chigraph>`_ is a visual programming language for beginners that is unique in that it is an intuitive flow graph:

.. image:: https://github.com/chigraph/chigraph/blob/master/doc/screenshots/HelloWorld.png?raw=true


It features easy bindings to C/C++, package management, and a cool interface.


Spkgen particle editor
^^^^^^^^^^^^^^^^^^^^^^

`Spkgen <https://github.com/fredakilla/spkgen>`_ is an editor for the SPARK particles engine that uses a node-based interface to create particles effects for games

.. image:: https://camo.githubusercontent.com/bb6eb1387e296c17827c84b487f16f7d278b2c1313427287489320edc3e31286/68747470733a2f2f692e696d6775722e636f6d2f396f4557646a6c2e706e67
