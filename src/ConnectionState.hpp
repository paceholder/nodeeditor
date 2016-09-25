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

  ConnectionState(PortType port = PortType::None)
    : _requiredPort(port)
  {}

  ~ConnectionState();

public:

  void setRequiredPort(PortType end)
  { _requiredPort = end; }

  PortType requiredPort() const
  { return _requiredPort; }

  bool requiresPort() const
  { return _requiredPort != PortType::None; }

  void setNoRequiredPort()
  { _requiredPort = PortType::None; }

public:

  void interactWithNode(std::shared_ptr<Node> node);

  void setLastHoveredNode(std::shared_ptr<Node> node);

  std::shared_ptr<Node> const
  lastHoveredNode() const
  { return _lastHoveredNode.lock(); }

  void resetLastHoveredNode();

private:

  PortType _requiredPort;

  std::weak_ptr<Node> _lastHoveredNode;
};
