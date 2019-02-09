#include "NumberSourceDataModel.hpp"

#include <QtCore/QJsonValue>
#include <QtGui/QDoubleValidator>

#include "DecimalData.hpp"

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


QJsonObject
NumberSourceDataModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();

  if (_number)
    modelJson["number"] = QString::number(_number->number());

  return modelJson;
}


void
NumberSourceDataModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["number"];

  if (!v.isUndefined())
  {
    QString strNum = v.toString();

    bool   ok;
    double d = strNum.toDouble(&ok);
    if (ok)
    {
      _number = std::make_shared<DecimalData>(d);
      _lineEdit->setText(strNum);
    }
  }
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
    _number = std::make_shared<DecimalData>(number);

    Q_EMIT dataUpdated(0);
  }
  else
  {
    Q_EMIT dataInvalidated(0);
  }
}


NodeDataType
NumberSourceDataModel::
dataType(PortType, PortIndex) const
{
  return DecimalData().type();
}


std::shared_ptr<NodeData>
NumberSourceDataModel::
outData(PortIndex)
{
  return _number;
}
