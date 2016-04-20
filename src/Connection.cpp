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

struct
Connection::ConnectionImpl
{
  ConnectionImpl(Connection &connection)
    : _id(QUuid::createUuid())
    , _connectionState()
    , _connectionGraphicsObject(new ConnectionGraphicsObject(connection))
  {
    //
  }

  ~ConnectionImpl()
  {
    std::cout << "Remove ConnectionGraphicsObject from scene" << std::endl;
    FlowScene &flowScene = FlowScene::instance();

    flowScene.removeItem(_connectionGraphicsObject.get());
  }

  QUuid _id;

  std::pair<QUuid, int> _sourceAddress; // ItemID, entry number
  std::pair<QUuid, int> _sinkAddress;

  ConnectionState _connectionState;
  ConnectionGeometry _connectionGeometry;
  std::unique_ptr<ConnectionGraphicsObject> _connectionGraphicsObject;
};

//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------
//----------------------------------------------------------

Connection::
Connection()
  : _impl(new ConnectionImpl(*this))
{
  //
}


Connection::
~Connection()
{
  std::cout << "Connection destructor" << std::endl;

  auto &scene = FlowScene::instance();

  auto tryDisconnectNode =
    [&](PortType portType)
    {
      auto address = getAddress(portType);

      if (!address.first.isNull())
      {
        std::shared_ptr<Node> node = scene.getNode(address.first);


        if (node)
          node->disconnect(this, portType, address.second);
      }
    };

  tryDisconnectNode(PortType::IN);
  tryDisconnectNode(PortType::OUT);
}


QUuid
Connection::
id() const
{
  return _impl->_id;
}


void
Connection::
setRequiredPort(PortType dragging)
{
  _impl->_connectionState.setRequiredPort(dragging);

  _impl->_connectionGraphicsObject->grabMouse();

  switch (dragging)
  {
    case PortType::OUT:
      _impl->_sourceAddress = std::make_pair(QUuid(), -1);
      break;

    case PortType::IN:
      _impl->_sinkAddress = std::make_pair(QUuid(), -1);
      break;

    default:
      break;
  }
}


PortType
Connection::
requiredPort() const
{
  return _impl->_connectionState.requiredPort();
}


std::pair<QUuid, int>
Connection::
getAddress(PortType portType) const
{
  switch (portType)
  {
    case PortType::OUT:
      return _impl->_sourceAddress;
      break;

    case PortType::IN:
      return _impl->_sinkAddress;
      break;

    default:
      break;
  }
  return std::make_pair(QUuid(), 0);
}


void
Connection::
setAddress(PortType portType, std::pair<QUuid, int> address)
{
  switch (portType)
  {
    case PortType::OUT:
      _impl->_sourceAddress = address;
      break;

    case PortType::IN:
      _impl->_sinkAddress = address;
      break;

    default:
      break;
  }
}


bool
Connection::
tryConnectToNode(std::shared_ptr<Node> node, QPointF const& scenePoint)
{
  bool ok = node->canConnect(_impl->_connectionState,
                             scenePoint);

  if (ok)
  {
    auto address = node->connect(this, scenePoint);

    if (!address.first.isNull())
    {
      //auto p = node->connectionPointScenePosition(address,
      //_impl->_requiredPort);

      connectToNode(address);

      //------
      _impl->_connectionState.setNoRequiredPort();
    }
  }

  return ok;
}


void
Connection::
connectToNode(std::pair<QUuid, int> const &address)
{
  setAddress(_impl->_connectionState.requiredPort(), address);

  std::shared_ptr<Node> const node = FlowScene::instance().getNode(address.first);

  std::unique_ptr<NodeGraphicsObject> const & o = node->nodeGraphicsObject();
  NodeGeometry const & nodeGeometry = node->nodeGeometry();

  QPointF const scenePoint =
    nodeGeometry.connectionPointScenePosition(address.second,
                                              _impl->_connectionState.requiredPort(),
                                              o->sceneTransform());

  auto p = _impl->_connectionGraphicsObject->mapFromScene(scenePoint);

  _impl->_connectionGeometry.setEndPoint(_impl->_connectionState.requiredPort(), p);

  if (getAddress(oppositePort(_impl->_connectionState.requiredPort())).first.isNull())
  {
    _impl->_connectionGeometry.setEndPoint(oppositePort(_impl->_connectionState.requiredPort()), p);
  }

  _impl->_connectionGraphicsObject->update();
}


std::unique_ptr<ConnectionGraphicsObject> &
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
