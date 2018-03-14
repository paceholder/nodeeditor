#pragma once

#include <utility>
#include <QtCore/QUuid>
#include "make_unique.hpp"


namespace QtNodes
{

enum class PortType
{
  None,
  In,
  Out
};

static const int INVALID = -1;

using PortIndex = int;

struct Port
{
  PortType type;

  PortIndex index;

  Port()
    : type(PortType::None)
    , index(INVALID)
  {}

  Port(PortType t, PortIndex i)
    : type(t)
    , index(i)
  {}

  bool
  indexIsValid() { return index != INVALID; }

  bool
  portTypeIsValid() { return type != PortType::None; }
};

//using PortAddress = std::pair<QUuid, PortIndex>;

inline
PortType
oppositePort(PortType port)
{
  PortType result = PortType::None;

  switch (port)
  {
    case PortType::In:
      result = PortType::Out;
      break;

    case PortType::Out:
      result = PortType::In;
      break;

    default:
      break;
  }

  return result;
}
}
