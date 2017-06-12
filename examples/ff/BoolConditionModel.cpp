#include "BoolConditionModel.hpp"

#include <QtCore/QJsonValue>
#include <QtGui/QDoubleValidator>

#include "ExpressionRangeData.hpp"

#include <QtCore/QDebug>

BoolConditionModel::
BoolConditionModel()
  : _comboBox(new QComboBox())
  , _boolCondition(">")
{
  //_lineEdit->setValidator(new QDoubleValidator());

  _comboBox->addItem(" > ");
  _comboBox->addItem(" < ");


  connect(_comboBox, &QComboBox::currentTextChanged,
          this, &BoolConditionModel::onIndexChanged);


  qDebug() << "Constructed";
}


QJsonObject
BoolConditionModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();
#if 0
  if (_expression)
    modelJson["expression"] = _expression->expression();
#endif
  return modelJson;
}


void
BoolConditionModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["expression"];
#if 0
  if (!v.isUndefined())
  {
    QString str = v.toString();

    _expression = std::make_shared<ExpressionRangeData>(str);
    _lineEdit->setText(str);
  }
#endif
}


unsigned int
BoolConditionModel::
nPorts(PortType portType) const
{
  unsigned int result = 1;

  switch (portType)
  {
    case PortType::In:
      result = 2;
      break;

    case PortType::Out:
      result = 1;

    default:
      break;
  }

  return result;
}


void
BoolConditionModel::
onIndexChanged(QString const &string)
{
  Q_UNUSED(string);

  bool ok = false;

  QString text = _comboBox->currentText();

  if (!text.isEmpty())
  {
    _boolCondition = text;
    evaluate();
    emit dataUpdated(0);
  }
  else
  {
    emit dataInvalidated(0);
  }
}


void
BoolConditionModel::
setInData(std::shared_ptr<NodeData> data, int portIndex)
{
  auto numberData =
    std::dynamic_pointer_cast<ExpressionRangeData>(data);

  if (portIndex == 0)
  {
    _input1 = numberData;
  }
  else
  {
    _input2 = numberData;
  }

  evaluate();
}

void
BoolConditionModel::
evaluate()
{
  PortIndex const outPortIndex = 0;
  
  auto n1 = _input1.lock();
  auto n2 = _input2.lock();
  
  if (n1 && n2)
  {
    _expression = std::make_shared<ExpressionRangeData>(n1->expression() + 
                                                        _boolCondition +  n2->expression(), std::vector<double>());
  }
  else
  {
    _expression.reset();
  }
  
  emit dataUpdated(outPortIndex);
}


NodeDataType
BoolConditionModel::
dataType(PortType, PortIndex) const
{
  return ExpressionRangeData().type();
}


std::shared_ptr<NodeData>
BoolConditionModel::
outData(PortIndex)
{
  return _expression;
}
