#include "Node.hpp"

#include <iostream>

#include "FlowScene.hpp"

#include "NodeGraphicsObject.hpp"

class Node::NodeImpl
{
public:
  NodeImpl(Node* node)
    : _id(QUuid::createUuid())
    , _nodeState(std::rand() % 4 + 2,
                 std::rand() % 4 + 2)
    , _nodeGraphicsObject(new NodeGraphicsObject(*node,
          _nodeState,
                                                 _nodeGeometry))
  {
    _nodeGeometry.setNSources(_nodeState.getEntries(EndType::SOURCE).size());
    _nodeGeometry.setNSinks(_nodeState.getEntries(EndType::SINK).size());

    _nodeGeometry.recalculateSize();
  }

  ~NodeImpl()
  {
    std::cout << "About to delete graphics object" << std::endl;
    FlowScene &flowScene = FlowScene::instance();

    flowScene.removeItem(_nodeGraphicsObject);
    delete _nodeGraphicsObject;
  }

public:

  // addressing

  QUuid _id;

  NodeState _nodeState;

  // painting

  NodeGeometry _nodeGeometry;

  NodeGraphicsObject* _nodeGraphicsObject;
};

Node::
Node()
  : _impl(new NodeGraphicsObject())
{
//
}


Node::
~Node()
{
  //
}
