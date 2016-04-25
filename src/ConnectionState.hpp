#pragma once

#include <memory>

#include <QtCore/QUuid>

#include "PortType.hpp"

class QPointF;
class Node;

/// Stores currently draggind end.
/// Remembers last hovered Node.
class ConnectionState
{
public:

  ConnectionState(PortType port = PortType::NONE)
    : _requiredPort(port)
  {}

  ~ConnectionState();

public:

  void setRequiredPort(PortType end)
  { _requiredPort = end; }

  PortType requiredPort() const
  { return _requiredPort; }

  bool requiresPort() const
  { return _requiredPort != PortType::NONE; }

  void setNoRequiredPort()
  { _requiredPort = PortType::NONE; }

public:

  void interactWithNode(std::shared_ptr<Node> node, QPointF const& scenePos);

  void setLastHoveredNode(QUuid id);

  QUuid lastHoveredNode() const
  { return _lastHoveredNodeId; }

  void resetLastHoveredNode();

private:

  PortType _requiredPort;

  QUuid _lastHoveredNodeId;
};
