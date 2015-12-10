#ifndef END_TYPE_HPP
#define END_TYPE_HPP

enum class EndType
{
  NONE,
  SOURCE,
  SINK
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


#endif //  END_TYPE_HPP
