#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/NodeDataModel>

#include "MathOperationDataModel.hpp"

#include "NumberData.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class SubtractionModel : public MathOperationDataModel
{
public:

  virtual
  ~SubtractionModel() {}

public:

  QString
  caption() const override
  { return QString("Subtraction"); }

  virtual bool
  portCaptionVisible(PortType portType, PortIndex portIndex) const override
  { return true; }

  virtual QString
  portCaption(PortType portType, PortIndex portIndex) const override
  {
    switch (portType)
    {
    case PortType::In:
      if (portIndex == 0)
        return "Minuend";
      else if (portIndex == 1)
        return "Subtrahend";
      break;

    case PortType::Out:
      return "Result";

    default:
      break;
    }
    return "";
  }

  QString
  name() const override
  { return QString("Subtraction"); }

  std::unique_ptr<NodeDataModel>
  clone() const override
  { return std::make_unique<SubtractionModel>(); }

public:

  void
  save(Properties &p) const override
  {
    p.put("model_name", SubtractionModel::name());
  }

private:

  void
  compute() override
  {
    PortIndex const outPortIndex = 0;

    auto n1 = _number1.lock();
    auto n2 = _number2.lock();

    if (n1 && n2)
    {
      _result = std::make_shared<NumberData>(n1->number() -
                                             n2->number());
    }
    else
    {
      _result.reset();
    }

    emit dataUpdated(outPortIndex);
  }
};
