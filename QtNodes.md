Plans
=======================================================================

1. Model-view approach.
A specific graph representation is wrapped in a model that delivers the
information to the framework. The graph could be stored as a set of
pointers, table, etc.
2. Headless mode. Graph Model works on its own without GUI rendering.
3. Optional data propagation (data flow). 
4. QML frontend.


Graph Model
=======================================================================

Graph Model references all the nodes and their interconnections.
Nodes have input and output ports. We model directed graph.


Port Removing and Insertion
-----------------------------------------------------------------------

Removing

```
  1      1

  2      _
     ->
  3      3

  4      4
```

Insertion

```
  1      1

  _      2

  2      3

  3      4
```


Graph Visualization
-----------------------------------------------------------------------

A simple Graph Model could be visualized.
It does not support any data types or data transferring.
We show generic nodes and connections and enable simple editing:
- Moving nodes around,
- Dis- and Re-connecting nodes,


Node Geometry
-----------------------------------------------------------------------

```                  vertical spacing
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

```


`NodeGraphicsScene` stores the maps:
- `map<NodeId, NodeGraphicsObject>`,
- `map<ConnectionId, ConnectionGraphicsObject>`

Each NodeGraphicsObject gets all the relevant information from the
`GraphModel` (node position, styles, names and caption, etc).

```
class FlowGraphicsScene : public QGraphicsScene
{
public:
  explicit NodeGraphicsScene(GraphModel * graphModel) {}

  ...

};
```


```cpp

class GraphModel
{
public:

  std::vector<Node> const &
  allNodes() const = 0;

  unsigned int
  nPorts(Node const & node, PortType portType) const = 0;

  std::vector<Node>
  connections(Node const & node, PortType portType) const = 0;

  QVariant
  data(Node const & node, NodeRole role);

  void
  setData(Node const, NodeRole role);
};


enum class NodeRole
{
  Data     = 0,
  DataType = 1,
  ConnectionPolicy = 2,

  Position = 3,
  CaptionVisible = 4,
  Caption = 5,


}:

```

Node Creation and Deleting
======================================================================

Creation
----------------------------------------------------------------------


We must manipulate the GraphModel first.

Nodes are created when:
  - Manipulating the underlying graph.
    - Loading the data from outside.
  - Manipulate the visualized scene.

```
scene::createNode() -> graph::createNode()
```


