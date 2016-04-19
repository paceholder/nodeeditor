### Purpose

**NodeEditor** is conceived as a general-purpose Qt-based library aimed at graph-controlled data processing.  Nodes
represent algorithms with certain inputs and outputs. Connections transfer data from the output (source) of the first
node to the input (sink) of the second one.

**NodeEditor** is a model-driven framework. A library client defines models and registers them in the data model
registry. Further work is driven by notifications which are delivered to the user models. The notifications could be,
for example, of several types: `DataConnected`, `DataDisconnected`, `InputDataUpdated` etc.

The framework is a Visual [Dataflow Programming](https://en.wikipedia.org/wiki/Dataflow_programming) tool.
Each change in the source node is immediately propagated through all the connections updating  the whole graph.

### Current state

Currently library implements the simplest GUI interaction between nodes. It is possible to define a dummy model,
register it and create nodes in the scene. No data transfer is yet possible.

### Roadmap

1. Implement data transfer
2. Extend set of examples
3. Allow custom widget embedding

![Flow](https://bitbucket.org/paceholder/nodeeditor/raw/master/pictures/flow.png)

Youtube video:

[![Youtube demonstration](https://img.youtube.com/vi/PmJ1InmPMdE/0.jpg)](https://www.youtube.com/watch?v=PmJ1InmPMdE)