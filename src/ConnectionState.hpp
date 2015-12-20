#ifndef CONNECTION_STATE_HPP
#define CONNECTION_STATE_HPP

#include "EndType.hpp"

class ConnectionState
{
public:

  ConnectionState(EndType end = EndType::NONE)
    : _draggingEnd(end)
  {}

  void setDraggingEnd(EndType end)
  { _draggingEnd = end; }

  EndType draggingEnd() const
  { return _draggingEnd; }

  bool isDragging() const
  { return _draggingEnd != EndType::NONE; }

  void clearDragging()
  { _draggingEnd = EndType::NONE; }

private:
  EndType _draggingEnd;
};

#endif // CONNECTION_STATE_HPP
