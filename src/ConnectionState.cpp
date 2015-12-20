#include "ConnectionState.hpp"

#include <iostream>

#include "FlowScene.hpp"
#include "Node.hpp"

ConnectionState::
~ConnectionState()
{
  resetLastHoveredNode();
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
  Node * n    = scene.getNode(_lastHoveredNodeId);

  if (n)
    n->resetReactionToConnection();

  _lastHoveredNodeId = QUuid();
}
