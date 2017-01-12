#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/NodeDataModel>

#include "MathOperationDataModel.hpp"

#include "NumberData.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class DivisionModel : public MathOperationDataModel
{
public:

  virtual
  ~DivisionModel() {}

public:
  QString
  caption() const override
  { return QString("Division"); }
  
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
          return "Dividend";
        else if (portIndex == 1)
          return "Divisor";
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
  { return QString("Division"); }

  std::unique_ptr<NodeDataModel>
  clone() const override
  { return std::make_unique<DivisionModel>(); }

public:

  void
  save(Properties &p) const override
  {
    p.put("model_name", DivisionModel::name());
  }

private:

  void
  compute() override
  {
    PortIndex const outPortIndex = 0;

    auto n1 = _number1.lock();
    auto n2 = _number2.lock();

    if (n1 && n2 && (n2->number() != 0.0))
    {
      _result = std::make_shared<NumberData>(n1->number() /
                                             n2->number());
    }
    else
    {
      _result.reset();
    }

    emit dataUpdated(outPortIndex);
  }
};
