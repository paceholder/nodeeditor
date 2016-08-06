#include "TextDisplayDataModel.hpp"

// For some reason CMake could not generate moc-files correctly
// without having a cpp for an QObject from hpp.

TextDisplayDataModel::
TextDisplayDataModel()
  : _label(new QLabel("Default Text"))
{
  //
}


unsigned int
TextDisplayDataModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::IN:
      result = 1;
      break;

    case PortType::OUT:
      result = 0;

    default:
      break;
  }

  return result;
}


NodeDataType
TextDisplayDataModel::
dataType(PortType, PortIndex) const
{
  return TextData().type();
}


std::shared_ptr<NodeData>
TextDisplayDataModel::
outData(PortIndex)
{
  return std::make_shared<TextData>();
}
