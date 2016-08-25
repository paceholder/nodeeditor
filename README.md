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
and create nodes in the scene. Data propagation works.

### Dependencies

The project uses Qt 5.5 and CMake 3.4

### Roadmap

1. Extend set of examples
2. Improve GUI: node names, resizing, node window scrolling and positioning
3. Check Windows builds


### Youtube video:

[![Youtube demonstration](https://img.youtube.com/vi/pxMXjSvlOFw/0.jpg)](https://www.youtube.com/watch?v=pxMXjSvlOFw)

[![Youtube demonstration](https://img.youtube.com/vi/PmJ1InmPMdE/0.jpg)](https://www.youtube.com/watch?v=PmJ1InmPMdE)


![Flow](https://bitbucket.org/paceholder/nodeeditor/raw/master/pictures/flow.png)

![Calculator](https://bitbucket.org/paceholder/nodeeditor/raw/master/pictures/calculator.png)

