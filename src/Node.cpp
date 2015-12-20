#include "Node.hpp"

#include <QtCore/QObject>

#include <iostream>

#include "FlowScene.hpp"

#include "NodeState.hpp"
#include "NodeGeometry.hpp"
#include "NodeGraphicsObject.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

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

//------------------------------------------------------------------------------

Node::
Node()
  : _impl(new NodeImpl(this))
{
//
}


Node::
~Node()
{
  //
}


QUuid const
Node::
id() const
{
  return _impl->_id;
}


void
Node::
reactToPossibleConnection(EndType,
                          QPointF const &scenePoint)
{
  QTransform const t = _impl->_nodeGraphicsObject->sceneTransform();

  QPointF p = t.inverted().map(scenePoint);

  _impl->_nodeGeometry.setDraggingPosition(p);
}


void
Node::
update()
{
  _impl->_nodeGraphicsObject->update();
}


bool
Node::
canConnect(ConnectionState const& conState,
           QPointF const &scenePoint)
{
  auto g   = _impl->_nodeGraphicsObject;
  int  hit =
    _impl->_nodeGeometry.checkHitScenePoint(conState.draggingEnd(),
                                            scenePoint,
                                            _impl->_nodeState,
                                            g->sceneTransform());

  return ((hit >= 0) &&
          _impl->_nodeState.connectionID(conState.draggingEnd(), hit).isNull());
}


std::pair<QUuid, int>
Node::
connect(Connection const* connection, int hit)
{
  ConnectionState const& conState =
    connection->connectionState();

  _impl->_nodeState.setConnectionId(conState.draggingEnd(),
                                    hit,
                                    connection->id());

  QObject::connect(_impl->_nodeGraphicsObject,
                   &NodeGraphicsObject::itemMoved,
                   connection->getConnectionGraphicsObject(),
                   &ConnectionGraphicsObject::onItemMoved);

  connection->getConnectionGraphicsObject()->setZValue(-1.0);

  return std::make_pair(_impl->_id, hit);
}


std::pair<QUuid, int>
Node::
connect(Connection const* connection,
        QPointF const & scenePoint)
{
  ConnectionState const& conState =
    connection->connectionState();

  auto g   = _impl->_nodeGraphicsObject;
  int  hit =
    _impl->_nodeGeometry.checkHitScenePoint(conState.draggingEnd(),
                                            scenePoint,
                                            _impl->_nodeState,
                                            g->sceneTransform());

  return connect(connection, hit);
}


void
Node::
disconnect(Connection const* connection,
           EndType endType,
           int hit)
{
  QObject::disconnect(_impl->_nodeGraphicsObject,
                      &NodeGraphicsObject::itemMoved,
                      connection->getConnectionGraphicsObject(),
                      &ConnectionGraphicsObject::onItemMoved);

  _impl->_nodeState.setConnectionId(endType, hit, QUuid());
}


NodeGraphicsObject const*
Node::
nodeGraphicsObject() const
{
  return _impl->_nodeGraphicsObject;
}


NodeGeometry&
Node::
nodeGeometry()
{
  return _impl->_nodeGeometry;
}


NodeGeometry const&
Node::
nodeGeometry() const
{
  return _impl->_nodeGeometry;
}
