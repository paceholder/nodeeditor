#pragma once

#include <utility>

#include <QtCore/QUuid>

enum class PortType
{
  NONE,
  IN,
  OUT
};


static const int INVALID = -1;

using PortIndex = int;

struct Port
{
  PortType type;

  PortIndex index;

  Port()
    : type(PortType::NONE)
    , index(INVALID)
  {}

  Port(PortType t, PortIndex i)
    : type(t)
    , index(i)
  {}

  bool indexIsValid() { return index != INVALID; }

  bool portTypeIsValid() { return type != PortType::NONE; }
};


//using PortAddress = std::pair<QUuid, PortIndex>;

static
PortType
oppositePort(PortType port)
{
  PortType result = PortType::NONE;

  switch (port)
  {
    case PortType::IN:
      result = PortType::OUT;
      break;

    case PortType::OUT:
      result = PortType::IN;
      break;

    default:
      break;
  }

  return result;
}

