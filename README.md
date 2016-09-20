Linux (x64, gcc, clang): [![Build Status](https://travis-ci.org/paceholder/nodeeditor.svg?branch=master)](https://travis-ci.org/paceholder/nodeeditor)
Windows (Win32, x64, msvc2015): [![Build status](https://ci.appveyor.com/api/projects/status/wxp47wv3uyyiujjw?svg=true)](https://ci.appveyor.com/project/paceholder/nodeeditor)

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

### Current state

Currently library implements the simplest GUI interaction between nodes. It is possible to define a model, register it
and create nodes in the scene. Data propagation works as well.

### Dependencies

The project uses Qt 5.5 and CMake 3.2

### Platforms

The code was compiled and tested in

1. Ubuntu Linux 16.04 with GCC 5.3 and Qt 5.7
2. Microsoft Windows 10 Pro with Visual Studio 2015 and Qt 5.7


### Roadmap

1. Extend set of examples
2. GUI: implement node resizing
3. GUI: fix scrolling for scene view window scrolling
4. Implement grouping nodes
4. Make Windows builds


### Youtube video:

[![Youtube demonstration](https://bitbucket.org/paceholder/nodeeditor/raw/master/pictures/vid1.png)](https://www.youtube.com/watch?v=pxMXjSvlOFw)

[![Youtube demonstration](https://img.youtube.com/vi/PmJ1InmPMdE/0.jpg)](https://www.youtube.com/watch?v=PmJ1InmPMdE)

![Flow](https://bitbucket.org/paceholder/nodeeditor/raw/master/pictures/flow.png)

![Calculator](https://bitbucket.org/paceholder/nodeeditor/raw/master/pictures/calculator.png)

