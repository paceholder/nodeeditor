#pragma once

enum class EndType
{
  NONE,
  SOURCE,
  SINK
};

enum PortNumber
{
  INVALID = -1
};

static
EndType
oppositeEnd(EndType end)
{
  EndType result = EndType::NONE;

  switch (end)
  {
    case EndType::SINK:
      result = EndType::SOURCE;
      break;

    case EndType::SOURCE:
      result = EndType::SINK;
      break;

    default:
      break;
  }

  return result;
}

