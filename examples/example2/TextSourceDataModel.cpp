#include "TextSourceDataModel.hpp"

TextSourceDataModel::
TextSourceDataModel()
  : _lineEdit(new QLineEdit("Default Text")),
    _textData(std::make_shared<TextData>())
{
  connect(_lineEdit, &QLineEdit::textEdited,
          this, &TextSourceDataModel::onTextEdited);
}


unsigned int
TextSourceDataModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 0;
      break;

    case PortType::Out:
      result = 1;

    default:
      break;
  }

  return result;
}


void
TextSourceDataModel::
onTextEdited(QString const &string)
{
  Q_UNUSED(string);

  Q_EMIT dataUpdated(0);
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
  *_textData = TextData(_lineEdit->text());
  return _textData;
}
