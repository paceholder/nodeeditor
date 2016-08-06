#include "TextSourceDataModel.hpp"

// For some reason CMake could not generate moc-files correctly
// without having a cpp for an QObject from hpp.

TextSourceDataModel::
TextSourceDataModel()
  : _lineEdit(new QLineEdit("Default Text"))
{
  //
}


unsigned int
TextSourceDataModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::IN:
      result = 0;
      break;

    case PortType::OUT:
      result = 1;

    default:
      break;
  }

  return result;
}


NodeDataType
TextSourceDataModel::
dataType(PortType, PortIndex) const
{
  return TextData().type();
}


std::shared_ptr<NodeData>
TextSourceDataModel::
outData(PortIndex)
{
  return std::make_shared<TextData>();
}
