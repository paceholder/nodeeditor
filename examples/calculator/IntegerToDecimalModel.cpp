#include "IntegerToDecimalModel.hpp"

#include <QtGui/QDoubleValidator>

#include "DecimalData.hpp"
#include "IntegerData.hpp"

QJsonObject
IntegerToDecimalModel::
save() const
{
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}


unsigned int
IntegerToDecimalModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 1;
      break;

    case PortType::Out:
      result = 1;

    default:
      break;
  }

  return result;
}


NodeDataType
IntegerToDecimalModel::
dataType(PortType portType, PortIndex) const
{
  if (portType == PortType::In)
    return IntegerData().type();

  return DecimalData().type();
}


std::shared_ptr<NodeData>
IntegerToDecimalModel::
outData(PortIndex)
{
  return _decimal;
}


void
IntegerToDecimalModel::
setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
  auto numberData =
    std::dynamic_pointer_cast<IntegerData>(data);

  if (portIndex == 0)
  {
    _integer = numberData;
  }

  if (_integer)
    _decimal = std::make_shared<DecimalData>(_integer->number());

  PortIndex const outPortIndex = 0;

  emit dataUpdated(outPortIndex);
}
