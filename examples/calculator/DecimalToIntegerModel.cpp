#include "DecimalToIntegerModel.hpp"

#include <QtGui/QDoubleValidator>

#include "DecimalData.hpp"
#include "IntegerData.hpp"

QJsonObject
DecimalToIntegerModel::
save() const
{
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}


unsigned int
DecimalToIntegerModel::
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
DecimalToIntegerModel::
dataType(PortType portType, PortIndex) const
{
  if (portType == PortType::In)
    return DecimalData().type();

  return IntegerData().type();
}


std::shared_ptr<NodeData>
DecimalToIntegerModel::
outData(PortIndex)
{
  return _integer;
}


void
DecimalToIntegerModel::
setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
  auto numberData =
    std::dynamic_pointer_cast<DecimalData>(data);

  if (portIndex == 0)
  {
    _decimal = numberData;
  }

  if (_decimal)
    _integer = std::make_shared<IntegerData>(_decimal->number());

  PortIndex const outPortIndex = 0;

  emit dataUpdated(outPortIndex);
}
