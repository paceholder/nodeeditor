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

  virtual ~SubtractionModel() {}

  QString modelName() const override
  { return QString("Subtraction"); }

private:

  void compute() override
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
