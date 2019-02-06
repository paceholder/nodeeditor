#include "ModuloModel.hpp"

#include <QtGui/QDoubleValidator>

#include "IntegerData.hpp"

QJsonObject
ModuloModel::
save() const
{
  QJsonObject modelJson;

  modelJson["name"] = name();

  return modelJson;
}


unsigned int
ModuloModel::
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


NodeDataType
ModuloModel::
dataType(PortType, PortIndex) const
{
  return IntegerData().type();
}


std::shared_ptr<NodeData>
ModuloModel::
outData(PortIndex)
{
  return _result;
}


void
ModuloModel::
setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
  auto numberData =
    std::dynamic_pointer_cast<IntegerData>(data);

  if (portIndex == 0)
  {
    _number1 = numberData;
  }
  else
  {
    _number2 = numberData;
  }

  {
    PortIndex const outPortIndex = 0;

    auto n1 = _number1.lock();
    auto n2 = _number2.lock();

    if (n2 && (n2->number() == 0.0))
    {
      modelValidationState = NodeValidationState::Error;
      modelValidationError = QStringLiteral("Division by zero error");
      _result.reset();
    }
    else if (n1 && n2)
    {
      modelValidationState = NodeValidationState::Valid;
      modelValidationError = QString();
      _result = std::make_shared<IntegerData>(n1->number() %
                                              n2->number());
    }
    else
    {
      modelValidationState = NodeValidationState::Warning;
      modelValidationError = QStringLiteral("Missing or incorrect inputs");
      _result.reset();
    }

    Q_EMIT dataUpdated(outPortIndex);
  }
}


NodeValidationState
ModuloModel::
validationState() const
{
  return modelValidationState;
}


QString
ModuloModel::
validationMessage() const
{
  return modelValidationError;
}
