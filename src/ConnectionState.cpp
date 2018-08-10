#include "ConnectionState.hpp"

#include <iostream>

#include <QtCore/QPointF>

#include "FlowScene.hpp"
#include "NodeGraphicsObject.hpp"

using QtNodes::ConnectionState;
using QtNodes::NodeGraphicsObject;

ConnectionState::
~ConnectionState()
{
  resetLastHoveredNode();
}


void
ConnectionState::
interactWithNode(NodeGraphicsObject* node)
{
  if (node)
  {
    _lastHoveredNode = node;
  }
  else
  {
    resetLastHoveredNode();
  }
}


void
ConnectionState::
setLastHoveredNode(NodeGraphicsObject* node)
{
  _lastHoveredNode = node;
}


void
ConnectionState::
resetLastHoveredNode()
{
  if (_lastHoveredNode)
    _lastHoveredNode->resetReactionToConnection();

  _lastHoveredNode = nullptr;
}
