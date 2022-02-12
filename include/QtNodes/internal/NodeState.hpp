#pragma once

#include <vector>
#include <unordered_map>

#include <QtCore/QPointF>
#include <QtCore/QUuid>

#include "Export.hpp"

#include "Definitions.hpp"
#include "NodeData.hpp"

namespace QtNodes
{

class ConnectionGraphicsObject;
class NodeGraphicsObject;

/// Stores bool for hovering connections and resizing flag.
class NODE_EDITOR_PUBLIC NodeState
{
public:

  NodeState(NodeGraphicsObject & ngo);

public:

  bool
  hovered() const
  { return _hovered; }

  void
  setHovered(bool hovered = true)
  { _hovered = hovered; }

  void
  setResizing(bool resizing);

  bool
  resizing() const;

  ConnectionGraphicsObject const *
  connectionForReaction() const;

  void
  storeConnectionForReaction(ConnectionGraphicsObject const * cgo);

  void
  resetConnectionForReaction();

private:

  NodeGraphicsObject & _ngo;

  bool _hovered;

  bool _resizing;

  ConnectionGraphicsObject const * _connectionForReaction;
};
}
