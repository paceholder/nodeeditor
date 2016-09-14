#include "NumberSourceDataModel.hpp"

#include <QtGui/QDoubleValidator>

#include "NumberData.hpp"

NumberSourceDataModel::
NumberSourceDataModel()
  : _lineEdit(new QLineEdit())
{
  _lineEdit->setValidator(new QDoubleValidator());

  _lineEdit->setMaximumSize(_lineEdit->sizeHint());

  connect(_lineEdit, &QLineEdit::textChanged,
          this, &NumberSourceDataModel::onTextEdited);

  _lineEdit->setText("0.0");
}


unsigned int
NumberSourceDataModel::
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
NumberSourceDataModel::
onTextEdited(QString const &string)
{
  Q_UNUSED(string);

  bool ok = false;

  double number = _lineEdit->text().toDouble(&ok);

  if (ok)
  {
    _number = std::make_shared<NumberData>(number);

    emit dataUpdated(0);
  }
  else
  {
    emit dataInvalidated(0);
  }
}


NodeDataType
NumberSourceDataModel::
dataType(PortType, PortIndex) const
{
  return NumberData().type();
}


std::shared_ptr<NodeData>
NumberSourceDataModel::
outData(PortIndex)
{
  return _number;
}
