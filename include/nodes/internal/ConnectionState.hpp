#pragma once

#include <QtCore/QUuid>

#include "PortType.hpp"

class QPointF;

namespace QtNodes
{

class NodeGraphicsObject;

/// Stores currently draggind end.
/// Remembers last hovered Node.
class ConnectionState
{
public:

  ConnectionState(PortType port = PortType::None)
    : _requiredPort(port)
  {}

  ConnectionState(const ConnectionState&) = delete;
  ConnectionState
  operator=(const ConnectionState&) = delete;

  ~ConnectionState();

public:

  void
  setRequiredPort(PortType end)
  { _requiredPort = end; }

  PortType
  requiredPort() const
  { return _requiredPort; }

  bool
  requiresPort() const
  { return _requiredPort != PortType::None; }

  void
  setNoRequiredPort()
  { _requiredPort = PortType::None; }

public:

  void
  interactWithNode(NodeGraphicsObject* node);

  void
  setLastHoveredNode(NodeGraphicsObject* node);

  NodeGraphicsObject*
  lastHoveredNode() const
  { return _lastHoveredNode; }

  void
  resetLastHoveredNode();

private:

  PortType _requiredPort;

  NodeGraphicsObject* _lastHoveredNode{nullptr};
};
}
