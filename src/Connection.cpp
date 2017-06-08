#include "Connection.hpp"

#include <iostream>
#include <math.h>

#include <QtWidgets/QtWidgets>
#include <QtGlobal>

#include "Node.hpp"
#include "FlowScene.hpp"
#include "FlowView.hpp"

#include "NodeGeometry.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeDataModel.hpp"

#include "ConnectionState.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionGraphicsObject.hpp"

using QtNodes::Connection;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::ConnectionState;
using QtNodes::Node;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::ConnectionGraphicsObject;
using QtNodes::ConnectionGeometry;

Connection::
Connection(PortType portType,
           Node& node,
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
Connection(Node& nodeIn,
           PortIndex portIndexIn,
           Node& nodeOut,
           PortIndex portIndexOut)
  : _id(QUuid::createUuid())
  , _outNode(&nodeOut)
  , _inNode(&nodeIn)
  , _outPortIndex(portIndexOut)
  , _inPortIndex(portIndexIn)
  , _connectionState()
{
  setNodeToPort(nodeIn, PortType::In, portIndexIn);
  setNodeToPort(nodeOut, PortType::Out, portIndexOut);
}


Connection::
~Connection()
{
  propagateEmptyData();

  if (_inNode)
  {
    _inNode->nodeGraphicsObject().update();
  }

  if (_outNode)
  {
    _outNode->nodeGraphicsObject().update();
  }
}


QJsonObject
Connection::
save() const
{
  QJsonObject connectionJson;

  if (_inNode && _outNode)
  {
    connectionJson["in_id"] = _inNode->id().toString();
    connectionJson["in_index"] = _inPortIndex;

    connectionJson["out_id"] = _outNode->id().toString();
    connectionJson["out_index"] = _outPortIndex;
  }

  return connectionJson;
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
    case PortType::Out:
      _outNode      = nullptr;
      _outPortIndex = INVALID;
      break;

    case PortType::In:
      _inNode      = nullptr;
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

  if (requiredPort() != PortType::None)
  {

    PortType attachedPort = oppositePort(requiredPort());

    PortIndex attachedPortIndex = getPortIndex(attachedPort);

    auto node = getNode(attachedPort);

    QTransform nodeSceneTransform =
      node->nodeGraphicsObject().sceneTransform();

    QPointF pos = node->nodeGeometry().portScenePosition(attachedPortIndex,
                                                         attachedPort,
                                                         nodeSceneTransform);

    _connectionGraphicsObject->setPos(pos);
  }

  _connectionGraphicsObject->move();
}


PortIndex
Connection::
getPortIndex(PortType portType) const
{
  PortIndex result = INVALID;

  switch (portType)
  {
    case PortType::In:
      result = _inPortIndex;
      break;

    case PortType::Out:
      result = _outPortIndex;

      break;

    default:
      break;
  }

  return result;
}


void
Connection::
setNodeToPort(Node& node,
              PortType portType,
              PortIndex portIndex)
{
  auto& nodeWeak = getNode(portType);

  nodeWeak = &node;

  if (portType == PortType::Out)
    _outPortIndex = portIndex;
  else
    _inPortIndex = portIndex;

  _connectionState.setNoRequiredPort();

  updated(*this);
}


void
Connection::
removeFromNodes() const
{
  if (_inNode)
    _inNode->nodeState().eraseConnection(PortType::In, _inPortIndex, id());

  if (_outNode)
    _outNode->nodeState().eraseConnection(PortType::Out, _outPortIndex, id());
}


ConnectionGraphicsObject&
Connection::
getConnectionGraphicsObject() const
{
  return *_connectionGraphicsObject;
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


ConnectionGeometry const&
Connection::
connectionGeometry() const
{
  return _connectionGeometry;
}


Node*
Connection::
getNode(PortType portType) const
{
  switch (portType)
  {
    case PortType::In:
      return _inNode;
      break;

    case PortType::Out:
      return _outNode;
      break;

    default:
      // not possible
      break;
  }
  return nullptr;
}


Node*&
Connection::
getNode(PortType portType)
{
  switch (portType)
  {
    case PortType::In:
      return _inNode;
      break;

    case PortType::Out:
      return _outNode;
      break;

    default:
      // not possible
      break;
  }
  Q_UNREACHABLE();
}


void
Connection::
clearNode(PortType portType)
{
  getNode(portType) = nullptr;

  if (portType == PortType::In)
    _inPortIndex = INVALID;
  else
    _outPortIndex = INVALID;
}


NodeDataType
Connection::
dataType() const
{
  Node* validNode;
  PortIndex index    = INVALID;
  PortType  portType = PortType::None;

  if ((validNode = _inNode))
  {
    index    = _inPortIndex;
    portType = PortType::In;
  }
  else if ((validNode = _outNode))
  {
    index    = _outPortIndex;
    portType = PortType::Out;
  }

  if (validNode)
  {
    auto const &model = validNode->nodeDataModel();

    return model->dataType(portType, index);
  }

  Q_UNREACHABLE();
}


void
Connection::
propagateData(std::shared_ptr<NodeData> nodeData) const
{
  if (_inNode)
  {
    _inNode->propagateData(nodeData, _inPortIndex);
  }
}


void
Connection::
propagateEmptyData() const
{
  std::shared_ptr<NodeData> emptyData;

  propagateData(emptyData);
}
