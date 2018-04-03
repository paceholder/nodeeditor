#include "Converters.hpp"

#include <QtGui/QDoubleValidator>

#include "DecimalData.hpp"
#include "IntegerData.hpp"



std::shared_ptr<NodeData>
DecimalToIntegerConverter::
operator()(std::shared_ptr<NodeData> data)
{
  auto numberData =
    std::dynamic_pointer_cast<DecimalData>(data);

  if (numberData)
  {
    _integer = std::make_shared<IntegerData>(numberData->number());
  }

  return _integer;
}


std::shared_ptr<NodeData>
IntegerToDecimalConverter::
operator()(std::shared_ptr<NodeData> data)
{
  auto numberData =
    std::dynamic_pointer_cast<IntegerData>(data);

  if (numberData)
  {
    _decimal = std::make_shared<DecimalData>(numberData->number());
  }

  return _decimal;
}

