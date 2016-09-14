#include "NumberDisplayDataModel.hpp"

#include "NumberData.hpp"

NumberDisplayDataModel::
NumberDisplayDataModel()
  : _label(new QLabel())
{
  _label->setMargin(3);
}


unsigned int
NumberDisplayDataModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 1;
      break;

    case PortType::Out:
      result = 0;

    default:
      break;
  }

  return result;
}


NodeDataType
NumberDisplayDataModel::
dataType(PortType, PortIndex) const
{
  return NumberData().type();
}


std::shared_ptr<NodeData>
NumberDisplayDataModel::
outData(PortIndex)
{
  std::shared_ptr<NodeData> ptr;
  return ptr;
}


void
NumberDisplayDataModel::
setInData(std::shared_ptr<NodeData> data, int)
{
  auto numberData = std::dynamic_pointer_cast<NumberData>(data);

  if (numberData)
  {
    _label->setText(numberData->numberAsText());
  }
  else
  {
    _label->clear();
  }

  _label->adjustSize();
}
