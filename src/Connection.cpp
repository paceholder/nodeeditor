#include "Connection.hpp"

#include <iostream>
#include <math.h>

#include <QtWidgets/QtWidgets>

#include "Node.hpp"
#include "FlowScene.hpp"
#include "FlowGraphicsView.hpp"

#include "NodeGeometry.hpp"
#include "NodeGraphicsObject.hpp"

#include "ConnectionState.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionGraphicsObject.hpp"

class
Connection::ConnectionImpl
{
public:
  ConnectionImpl(Connection* connection)
    : _id(QUuid::createUuid())
    , _connectionState()
    , _connectionGraphicsObject(new ConnectionGraphicsObject(*connection))
  {}

  ~ConnectionImpl()
  {

    std::cout << "About to delete graphics object" << std::endl;
    FlowScene &flowScene = FlowScene::instance();

    flowScene.removeItem(_connectionGraphicsObject);
    delete _connectionGraphicsObject;
  }

public:
  QUuid _id;

  std::pair<QUuid, int> _sourceAddress; // ItemID, entry number
  std::pair<QUuid, int> _sinkAddress;

  ConnectionState    _connectionState;
  ConnectionGeometry _connectionGeometry;
  ConnectionGraphicsObject* _connectionGraphicsObject;
};

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

Connection::
Connection()
  : _impl(new ConnectionImpl(this))
{
//
}


Connection::
~Connection()
{
  auto &scene = FlowScene::instance();

  std::cout << "Connection destructor" << std::endl;

  auto tryDisconnectNode =
    [&](EndType endType)
    {
      auto address = getAddress(endType);

      if (!address.first.isNull())
      {
        Node* node = scene.getNode(address.first);

        node->disconnect(this, endType, address.second);
      }
    };

  tryDisconnectNode(EndType::SINK);
  tryDisconnectNode(EndType::SOURCE);
}


QUuid
Connection::
id() const
{
  return _impl->_id;
}


void
Connection::
setDraggingEnd(EndType dragging)
{
  _impl->_connectionState.setDraggingEnd(dragging);

  _impl->_connectionGraphicsObject->grabMouse();

  switch (dragging)
  {
    case EndType::SOURCE:
      _impl->_sourceAddress = std::make_pair(QUuid(), -1);
      break;

    case EndType::SINK:
      _impl->_sinkAddress = std::make_pair(QUuid(), -1);
      break;

    default:
      break;
  }
}


EndType
Connection::
draggingEnd() const
{
  return _impl->_connectionState.draggingEnd();
}


std::pair<QUuid, int>
Connection::
getAddress(EndType endType) const
{
  switch (endType)
  {
    case EndType::SOURCE:
      return _impl->_sourceAddress;
      break;

    case EndType::SINK:
      return _impl->_sinkAddress;
      break;

    default:
      break;
  }
  return std::make_pair(QUuid(), 0);
}


void
Connection::
setAddress(EndType endType, std::pair<QUuid, int> address)
{
  switch (endType)
  {
    case EndType::SOURCE:
      _impl->_sourceAddress = address;
      break;

    case EndType::SINK:
      _impl->_sinkAddress = address;
      break;

    default:
      break;
  }
}


bool
Connection::
tryConnectToNode(Node* node, QPointF const& scenePoint)
{
  bool ok = node->canConnect(_impl->_connectionState,
                             scenePoint);

  if (ok)
  {
    auto address = node->connect(this, scenePoint);

    if (!address.first.isNull())
    {
      //auto p = node->connectionPointScenePosition(address,
      //_impl->_draggingEnd);

      connectToNode(address);

      //------
      _impl->_connectionState.clearDragging();
    }
  }

  return ok;
}


void
Connection::
connectToNode(std::pair<QUuid, int> const &address)
{
  setAddress(_impl->_connectionState.draggingEnd(), address);

  Node const* node = FlowScene::instance().getNode(address.first);

  NodeGraphicsObject const* o = node->nodeGraphicsObject();
  NodeGeometry const      & nodeGeometry = node->nodeGeometry();

  QPointF const scenePoint =
    nodeGeometry.connectionPointScenePosition(address.second,
                                              _impl->_connectionState.draggingEnd(),
                                              o->sceneTransform());

  auto p = _impl->_connectionGraphicsObject->mapFromScene(scenePoint);

  _impl->_connectionGeometry.setEndPoint(_impl->_connectionState.draggingEnd(), p);

  if (getAddress(oppositeEnd(_impl->_connectionState.draggingEnd())).first.isNull())
  {
    _impl->_connectionGeometry.setEndPoint(oppositeEnd(_impl->_connectionState.draggingEnd()), p);
  }

  _impl->_connectionGraphicsObject->update();
}


ConnectionGraphicsObject*
Connection::
getConnectionGraphicsObject() const
{
  return _impl->_connectionGraphicsObject;
}


ConnectionState&
Connection::
connectionState()
{
  return _impl->_connectionState;
}


ConnectionState const&
Connection::
connectionState() const
{
  return _impl->_connectionState;
}


ConnectionGeometry&
Connection::
connectionGeometry()
{
  return _impl->_connectionGeometry;
}
