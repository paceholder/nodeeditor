#include "ExpressionDisplayModel.hpp"

#include <QtCore/QJsonValue>
#include <QtGui/QDoubleValidator>

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QLabel>
#include <QtWidgets/QFormLayout>

#include "ExpressionRangeData.hpp"

#include <QtCore/QDebug>

ExpressionDisplayModel::
ExpressionDisplayModel()
{
  _widget = new QWidget();

  auto l = new QFormLayout();

  _variableLabel = new QLabel();
  _variableLabel->setMargin(3);
  _variableLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  _rangeLabel = new QLabel();
  _rangeLabel->setMargin(3);
  _rangeLabel->setFrameStyle(QFrame::Panel | QFrame::Sunken);

  l->addRow("Variable:", _variableLabel);
  l->addRow("Range:", _rangeLabel);

  _widget->setLayout(l);
}


QJsonObject
ExpressionDisplayModel::
save() const
{
  QJsonObject modelJson = NodeDataModel::save();

  if (_expression)
    modelJson["expression"] = _expression->expression();

  return modelJson;
}


void
ExpressionDisplayModel::
restore(QJsonObject const &p)
{
  QJsonValue v = p["expression"];

  if (!v.isUndefined())
  {
    QString str = v.toString();

    std::vector<double> d;
    d.push_back(0.0);

    _expression = std::make_shared<ExpressionRangeData>(str, d);
    _variableLabel->setText(str);
  }
}


unsigned int
ExpressionDisplayModel::
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
ExpressionDisplayModel::
dataType(PortType, PortIndex) const
{
  return ExpressionRangeData().type();
}


std::shared_ptr<NodeData>
ExpressionDisplayModel::
outData(PortIndex)
{
  return _expression;
}


QString
ExpressionDisplayModel::
convertRangeToText(std::vector<double> const &range) const
{
  QString result("(");

  for (std::size_t i = 0; i < range.size() - 1; ++i)
  {
    result = result + QString::number(range[i]) + ", ";
  }

  result = result + QString::number(range.back()) + ")";

  return result;
}

void
ExpressionDisplayModel::
setInData(std::shared_ptr<NodeData> nodeData, PortIndex portIndex)
{
  _expression = std::static_pointer_cast<ExpressionRangeData>(nodeData);

  if (_expression)
  {
    _variableLabel->setText(_expression->expression());
    _variableLabel->adjustSize();

    _rangeLabel->setText(convertRangeToText(_expression->range()));
    _rangeLabel->adjustSize();

    emit dataUpdated(0);
  }
}


QWidget *
ExpressionDisplayModel::
embeddedWidget()
{
  return _widget;
}
