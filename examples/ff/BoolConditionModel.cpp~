#include "ExpressionSourceModel.hpp"

#include <QtCore/QJsonValue>
#include <QtGui/QDoubleValidator>

#include "ExpressionData.hpp"

#include <QtCore/QDebug>

ExpressionSourceModel::
ExpressionSourceModel()
  : _lineEdit(new QLineEdit())
{
  //_lineEdit->setValidator(new QDoubleValidator());

  _lineEdit->setMaximumSize(_lineEdit->sizeHint());

  connect(_lineEdit, &QLineEdit::textChanged,
          this, &ExpressionSourceModel::onTextEdited);

  _lineEdit->setPlaceholderText("Your expression");
  _lineEdit->setText("");


  qDebug() << "Constructed";
}


QJsonObject
ExpressionSourceModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();

  if (_expression)
    modelJson["expression"] = _expression->expression();

  return modelJson;
}


void
ExpressionSourceModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["expression"];

  if (!v.isUndefined())
  {
    QString str = v.toString();

    _expression = std::make_shared<ExpressionData>(str);
    _lineEdit->setText(str);
  }
}


unsigned int
ExpressionSourceModel::
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
ExpressionSourceModel::
onTextEdited(QString const &string)
{
  Q_UNUSED(string);

  bool ok = false;

  QString text = _lineEdit->text();

  if (!text.isEmpty())
  {
    _expression = std::make_shared<ExpressionData>(text);
    emit dataUpdated(0);
  }
  else
  {
    emit dataInvalidated(0);
  }
}


NodeDataType
ExpressionSourceModel::
dataType(PortType, PortIndex) const
{
  return ExpressionData().type();
}


std::shared_ptr<NodeData>
ExpressionSourceModel::
outData(PortIndex)
{
  return _expression;
}
