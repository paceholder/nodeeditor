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
nSlots(PortType portType) const
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


std::shared_ptr<NodeData>
TextSourceDataModel::
data(PortType, int slot)
{
  Q_UNUSED(slot);

  return std::make_shared<TextData>();
}
