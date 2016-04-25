#include "ConnectionState.hpp"

#include <iostream>

#include <QtCore/QPointF>

#include "FlowScene.hpp"
#include "Node.hpp"

ConnectionState::
~ConnectionState()
{
  resetLastHoveredNode();
}


void
ConnectionState::
interactWithNode(std::shared_ptr<Node> node, QPointF const& scenePos)
{
  if (node)
  {
    node->reactToPossibleConnection(_requiredPort, scenePos);

    _lastHoveredNodeId = node->id();
  }
  else
  {
    resetLastHoveredNode();
  }
}


void
ConnectionState::
setLastHoveredNode(QUuid id)
{
  _lastHoveredNodeId = id;
}


void
ConnectionState::
resetLastHoveredNode()
{
  auto &scene = FlowScene::instance();
  std::shared_ptr<Node> n = scene.getNode(_lastHoveredNodeId);

  if (n)
    n->resetReactionToConnection();

  _lastHoveredNodeId = QUuid();
}
