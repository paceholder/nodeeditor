### Purpose

**NodeEditor** is conceived as a general-purpose Qt-based library aimed at graph-controlled data processing.  Nodes
represent algorithms with certain inputs and outputs. Connections transfer data from the output (source) of the first
node to the input (sink) of the second one.

**NodeEditor** framework is a Visual [Dataflow Programming](https://en.wikipedia.org/wiki/Dataflow_programming) tool.
A library client defines models and registers them in the data model registry.
Further work is driven by events taking place in DataModels and Nodes.
The model computing is triggered upon arriving of any new input data. The computed result is propagated to the output
connections. Each new connection fetches available data and propagates is further.

Each change in the source node is immediately propagated through all the connections updating  the whole graph.

### Platforms

* OSX (Apple Clang - LLVM 3.6), Linux (x64, gcc-5.0, clang-3.8): [![Build Status](https://travis-ci.org/paceholder/nodeeditor.svg?branch=master)](https://travis-ci.org/paceholder/nodeeditor)
* Windows (Win32, x64, msvc2015, MinGW 5.3): [![Build status](https://ci.appveyor.com/api/projects/status/wxp47wv3uyyiujjw/branch/master?svg=true)](https://ci.appveyor.com/project/paceholder/nodeeditor/branch/master)


### Dependencies

* Qt >5.2
* CMake 3.2

### Current state

* Model-based nodes
* Automatic data propagation
* Datatype-aware connections
* Embedded Qt widgets
* One-output to many-input connections
* JSON-based interface styles
* Saving scenes to JSON files

### Roadmap

1. Extend set of examples
2. GUI: fix scrolling for scene view window scrolling
3. Implement grouping nodes
4. Split graph and GUI parts
5. Build data propagation on top of the graph code

### Citing

    Dmitry Pinaev et al, Qt5 Node Editor, (2017), GitHub repository, https://github.com/paceholder/nodeeditor

BibTeX

    @misc{Pinaev2017,
      author = {Dmitry Pinaev et al},
      title = {Qt5 Node Editor},
      year = {2017},
      publisher = {GitHub},
      journal = {GitHub repository},
      howpublished = {\url{https://github.com/paceholder/nodeeditor}},
      commit = {e89ff3ffbdd8c6245c4c1f7e9f73276bf15c3e88}
    }


### Youtube video:

[![Youtube demonstration](https://bitbucket.org/paceholder/nodeeditor/raw/master/pictures/vid1.png)](https://www.youtube.com/watch?v=pxMXjSvlOFw)

### Now with styles


[![Styles](https://bitbucket.org/paceholder/nodeeditor/raw/master/pictures/style_example.png)](https://www.youtube.com/watch?v=i_pB-Y0hCYQ)


### Buy me a beer

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://paypal.me/DmitryPinaev)

### Showcase

#### [Chigraph](https://github.com/chigraph/chigraph)

Chigraph is a programming language for beginners that is unique in that it is an intuitive flow graph:

![chigraph screenshot](pictures/chigraph.png)

It features easy bindings to C/C++, package management, and a cool interface.
