#include "Connection.hpp"

#include <iostream>
#include <math.h>

#include <QtWidgets/QtWidgets>

#include "Node.hpp"
#include "NodeData.hpp"
#include "FlowScene.hpp"
#include "FlowGraphicsView.hpp"

#include "NodeGeometry.hpp"
#include "NodeGraphicsObject.hpp"

#include "ConnectionState.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionGraphicsObject.hpp"

//----------------------------------------------------------

Connection::
Connection(PortType portType,
           std::shared_ptr<Node> node,
           PortIndex portIndex)
  : _id(QUuid::createUuid())
  , _outPortIndex(INVALID)
  , _inPortIndex(INVALID)
  , _connectionState()
{
  setNodeToPort(node, portType, portIndex);

  setRequiredPort(oppositePort(portType));
}


Connection::
~Connection()
{
  std::cout << "Connection destructor" << std::endl;
}


QUuid
Connection::
id() const
{
  return _id;
}


void
Connection::
setRequiredPort(PortType dragging)
{
  _connectionState.setRequiredPort(dragging);

  switch (dragging)
  {
    case PortType::OUT:
      _outNode.reset();
      _outPortIndex = INVALID;
      break;

    case PortType::IN:
      _inNode.reset();
      _inPortIndex = INVALID;
      break;

    default:
      break;
  }
}


PortType
Connection::
requiredPort() const
{
  return _connectionState.requiredPort();
}


void
Connection::
setGraphicsObject(std::unique_ptr<ConnectionGraphicsObject>&& graphics)
{
  _connectionGraphicsObject = std::move(graphics);

  // This function is only called when the ConnectionGraphicsObject
  // is newly created. At this moment both end coordinates are (0, 0)
  // in Connection G.O. coordinates. The position of the whole
  // Connection G. O. in scene coordinate system is also (0, 0).
  // By moving the whole object to the Node Port position
  // we position both connection ends correctly.

  PortType attachedPort = oppositePort(requiredPort());

  PortIndex attachedPortIndex = getPortIndex(attachedPort);

  std::shared_ptr<Node> node = getNode(attachedPort).lock();

  QTransform nodeSceneTransform =
    node->nodeGraphicsObject()->sceneTransform();

  QPointF pos = node->nodeGeometry().portScenePosition(attachedPortIndex,
                                                       attachedPort,
                                                       nodeSceneTransform);

  _connectionGraphicsObject->setPos(pos);
}


PortIndex
Connection::
getPortIndex(PortType portType) const
{
  PortIndex result = INVALID;

  switch (portType)
  {
    case PortType::IN:
      result = _inPortIndex;
      break;

    case PortType::OUT:
      result = _outPortIndex;

      break;

    default:
      break;
  }

  return result;
}


void
Connection::
setNodeToPort(std::shared_ptr<Node> node,
              PortType portType,
              PortIndex portIndex)
{
  std::weak_ptr<Node> & nodeWeak = getNode(portType);

  nodeWeak = node;

  if (portType == PortType::OUT)
    _outPortIndex = portIndex;
  else
    _inPortIndex = portIndex;

  _connectionState.setNoRequiredPort();
}


std::unique_ptr<ConnectionGraphicsObject> const&
Connection::
getConnectionGraphicsObject() const
{
  return _connectionGraphicsObject;
}


ConnectionState&
Connection::
connectionState()
{
  return _connectionState;
}


ConnectionState const&
Connection::
connectionState() const
{
  return _connectionState;
}


ConnectionGeometry&
Connection::
connectionGeometry()
{
  return _connectionGeometry;
}


std::weak_ptr<Node> const &
Connection::
getNode(PortType portType) const
{
  switch (portType)
  {
    case PortType::IN:
      return _inNode;
      break;

    case PortType::OUT:
      return _outNode;
      break;

    default:
      // not possible
      break;
  }
}


std::weak_ptr<Node> &
Connection::
getNode(PortType portType)
{
  switch (portType)
  {
    case PortType::IN:
      return _inNode;
      break;

    case PortType::OUT:
      return _outNode;
      break;

    default:
      // not possible
      break;
  }
}


void
Connection::
clearNode(PortType portType)
{
  getNode(portType).reset();

  if (portType == PortType::IN)
    _inPortIndex = INVALID;
  else
    _outPortIndex = INVALID;
}


void
Connection::
propagateData(std::shared_ptr<NodeData> nodeData) const
{
  auto inNode = _inNode.lock();

  if (inNode)
  {
    inNode->propagateData(nodeData, _inPortIndex);
  }
}
