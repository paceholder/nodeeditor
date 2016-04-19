#include "Node.hpp"

#include <QtCore/QObject>

#include <iostream>

#include "FlowScene.hpp"

#include "NodeState.hpp"
#include "NodeGeometry.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeDataModel.hpp"

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

struct Node::NodeImpl
{
  NodeImpl(Node &node,
           std::unique_ptr<NodeDataModel> &&dataModel)
    : _id(QUuid::createUuid())
    , _nodeDataModel(std::move(dataModel))
    , _nodeState(_nodeDataModel)
      , _nodeGeometry(_nodeDataModel)
    , _nodeGraphicsObject(new NodeGraphicsObject(node,
                                                 _nodeState,
                                                 _nodeGeometry))
  {
    _nodeGeometry.recalculateSize();
  }

  /// Destructor
  ~NodeImpl()
  {
    std::cout << "Remove NodeGraphicsObject from scene" << std::endl;
    FlowScene &flowScene = FlowScene::instance();

    flowScene.removeItem(_nodeGraphicsObject.get());
  }

  // addressing

  QUuid _id;

  // data

  std::unique_ptr<NodeDataModel> _nodeDataModel;

  NodeState _nodeState;

  // painting

  NodeGeometry _nodeGeometry;

  std::unique_ptr<NodeGraphicsObject> _nodeGraphicsObject;
};

//------------------------------------------------------------------------------

Node::
Node(std::unique_ptr<NodeDataModel> &&dataModel)
  : _impl(new NodeImpl(*this, std::move(dataModel)))
{
  //
}


Node::
~Node()
{
  //
}


QUuid
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

  _impl->_nodeGraphicsObject->update();

  _impl->_nodeState.setReaction(NodeState::REACTING);
}


void
Node::
resetReactionToConnection()
{
  _impl->_nodeState.setReaction(NodeState::NOT_REACTING);
  _impl->_nodeGraphicsObject->update();
}


bool
Node::
canConnect(ConnectionState const& conState,
           QPointF const &scenePoint)
{
  auto &g = _impl->_nodeGraphicsObject;

  int hit =
    _impl->_nodeGeometry.checkHitScenePoint(conState.draggingEnd(),
                                            scenePoint,
                                            _impl->_nodeState,
                                            g->sceneTransform());

  return ((hit != INVALID) &&
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

  QObject::connect(_impl->_nodeGraphicsObject.get(),
                   &NodeGraphicsObject::itemMoved,
                   connection->getConnectionGraphicsObject().get(),
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

  auto &g  = _impl->_nodeGraphicsObject;
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
  QObject::disconnect(_impl->_nodeGraphicsObject.get(),
                      &NodeGraphicsObject::itemMoved,
                      connection->getConnectionGraphicsObject().get(),
                      &ConnectionGraphicsObject::onItemMoved);

  _impl->_nodeState.setConnectionId(endType, hit, QUuid());

  _impl->_nodeGraphicsObject->update();
}


std::unique_ptr<NodeGraphicsObject> const &
Node::
nodeGraphicsObject() const
{
  return _impl->_nodeGraphicsObject;
}

std::unique_ptr<NodeGraphicsObject>&
Node::
nodeGraphicsObject()
{
  return _impl->_nodeGraphicsObject;
}



NodeGeometry&
Node::
nodeGeometry()
{
  return _impl->_nodeGeometry;
}


NodeGeometry&
Node::
nodeGeometry() const
{
  return _impl->_nodeGeometry;
}


NodeState const &
Node::
nodeState() const
{
  return _impl->_nodeState;
}


std::unique_ptr<NodeDataModel> const &
Node::
nodeDataModel() const
{
  return _impl->_nodeDataModel;
}
