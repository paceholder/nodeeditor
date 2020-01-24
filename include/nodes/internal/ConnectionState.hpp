#pragma once

#include <QtCore/QUuid>

#include "Export.hpp"

#include "Definitions.hpp"

class QPointF;

namespace QtNodes
{

class ConnectionGraphicsObject;

/// Stores currently draggind end.
/// Remembers last hovered Node.
class NODE_EDITOR_PUBLIC ConnectionState
{
public:

  /// Defines whether we construct a new connection
  /// or it is already binding two nodes.
  enum LooseEnd
  {
    Pending   = 0,
    Connected = 1
  };

public:

  ConnectionState(ConnectionGraphicsObject & cgo)
    : _cgo(cgo)
    , _connectedState(LooseEnd::Pending)
    , _hovered(false)
  {}

  ConnectionState(ConnectionState const&) = delete;
  ConnectionState(ConnectionState &&) = delete;

  ConnectionState& operator=(ConnectionState const&) = delete;
  ConnectionState& operator=(ConnectionState &&) = delete;

  ~ConnectionState();

public:

  PortType requiredPort() const;
  bool requiresPort() const;

  bool hovered() const { return _hovered; }
  void setHovered(bool hovered) { _hovered = hovered; }

public:

  /// Caches NodeId for further interaction.
  void interactWithNode(NodeId const nodeId);

  void setLastHoveredNode(NodeId const nodeId);

  NodeId
  lastHoveredNode() const
  { return _lastHoveredNode; }

  void resetLastHoveredNode();

private:

  ConnectionGraphicsObject & _cgo;

  LooseEnd _connectedState;

  bool _hovered;

  NodeId _lastHoveredNode{InvalidNodeId};

};
}
