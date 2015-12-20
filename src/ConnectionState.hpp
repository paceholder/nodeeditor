#ifndef CONNECTION_STATE_HPP
#define CONNECTION_STATE_HPP

#include <QtCore/QUuid>

#include "EndType.hpp"

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

  //----

  void setLastHoveredNode(QUuid id);

  QUuid lastHoveredNode() const
  { return _lastHoveredNodeId; }

  void resetLastHoveredNode();

private:
  EndType _draggingEnd;

  QUuid _lastHoveredNodeId;
};

#endif // CONNECTION_STATE_HPP
