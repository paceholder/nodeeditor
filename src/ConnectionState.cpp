#include "ConnectionState.hpp"

#include <QtCore/QDebug>
#include <QtCore/QPointF>

#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "NodeGraphicsObject.hpp"

namespace QtNodes
{

ConnectionState::
~ConnectionState()
{
  //resetLastHoveredNode();
}


PortType
ConnectionState::
requiredPort() const
{
  PortType t = PortType::None;

  if (std::get<0>(_cgo.connectionId()) == InvalidNodeId)
  {
    t = PortType::Out;
  }
  else
  {
    t = PortType::In;
  }

  return t;
}


bool
ConnectionState::
requiresPort() const
{
  ConnectionId id = _cgo.connectionId();
  return std::get<0>(id) == InvalidNodeId ||
         std::get<2>(id) == InvalidNodeId;
}


bool
ConnectionState::
hovered() const
{
  return _hovered;
}


void
ConnectionState::
setHovered(bool hovered)
{
  _hovered = hovered;
}


void
ConnectionState::
setLastHoveredNode(NodeId const nodeId)
{
  _lastHoveredNode = nodeId;
}


NodeId
ConnectionState::
lastHoveredNode() const
{
  return _lastHoveredNode;
}


void
ConnectionState::
resetLastHoveredNode()
{
  if (_lastHoveredNode != InvalidNodeId)
  {
    auto ngo =
      _cgo.nodeScene()->nodeGraphicsObject(_lastHoveredNode);
    ngo->update();
  }

  _lastHoveredNode = InvalidNodeId;
}


}
