#include "ExpressionSourceModel.hpp"

#include <QtCore/QJsonValue>
#include <QtGui/QDoubleValidator>

#include <QtWidgets/QWidget>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QFormLayout>

#include "ExpressionRangeData.hpp"

#include <QtCore/QDebug>

ExpressionSourceModel::
ExpressionSourceModel()
{
  _widget = new QWidget();

  auto l = new QFormLayout();

  _variableEdit = new QLineEdit();
  _variableEdit->setPlaceholderText("Variable name");

  _rangeEdit = new QLineEdit();
  _rangeEdit->setPlaceholderText("Comma-separated numbers");

  l->addRow("Variable", _variableEdit);
  l->addRow("Range", _rangeEdit);

  _widget->setLayout(l);

  //_variableEdit->setMaximumSize(_variableEdit->sizeHint());

  connect(_variableEdit, &QLineEdit::textChanged,
          this, &ExpressionSourceModel::onVariableEdited);

  connect(_rangeEdit, &QLineEdit::textChanged,
          this, &ExpressionSourceModel::onRangeEdited);


  //qDebug() << "Constructed";
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

    std::vector<double> range;
    range.push_back(0.0);

    _expression = std::make_shared<ExpressionRangeData>(str, range);
    _variableEdit->setText(str);
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


std::vector<double>
ExpressionSourceModel::
processRangeText(QString const &rangeText) const
{
  std::vector<double> result;

  QStringList numbers = rangeText.split(",", QString::SkipEmptyParts);


  bool ok = true;

  for(QString const & s : numbers)
  {
    bool ook;

    double d = s.toDouble(&ook);

    ok = ok && ook;

    if (ook)
    {
      result.push_back(d);
    }
  }

  if (ok)
    return result;

  return std::vector<double>();
}



void
ExpressionSourceModel::
processChangedData()
{
  bool ok = false;

  QString text = _variableEdit->text();

  std::vector<double> range = processRangeText(_rangeEdit->text());

  if (!text.isEmpty() && (range.size() > 0))
  {
    _expression = std::make_shared<ExpressionRangeData>(text, range);

    emit dataUpdated(0);
  }
  else
  {
    emit dataInvalidated(0);
  }
}


void
ExpressionSourceModel::
onVariableEdited(QString const &string)
{
  Q_UNUSED(string);

  processChangedData();
}


void
ExpressionSourceModel::
onRangeEdited(QString const &string)
{
  Q_UNUSED(string);

  processChangedData();
}


NodeDataType
ExpressionSourceModel::
dataType(PortType, PortIndex) const
{
  return ExpressionRangeData().type();
}


std::shared_ptr<NodeData>
ExpressionSourceModel::
outData(PortIndex)
{
  return _expression;
}


QWidget *
ExpressionSourceModel::
embeddedWidget()
{
  return _widget;
}
