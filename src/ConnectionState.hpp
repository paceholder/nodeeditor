#pragma once

#include <QtCore/QUuid>

#include "EndType.hpp"


/// Stores currently draggind end.
/// Remembers last hovered Node.
class ConnectionState
{
public:

  ConnectionState(EndType end = EndType::NONE)
    : _draggingEnd(end)
  {}

  ~ConnectionState();

  void setDraggingEnd(EndType end)
  { _draggingEnd = end; }

  EndType draggingEnd() const
  { return _draggingEnd; }

  bool isDragging() const
  { return _draggingEnd != EndType::NONE; }

  void clearDragging()
  { _draggingEnd = EndType::NONE; }

public:

  void setLastHoveredNode(QUuid id);

  QUuid lastHoveredNode() const
  { return _lastHoveredNodeId; }

  void resetLastHoveredNode();

private:

  EndType _draggingEnd;

  QUuid _lastHoveredNodeId;
};
