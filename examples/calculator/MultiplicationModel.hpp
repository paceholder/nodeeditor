#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/NodeDataModel>

#include "MathOperationDataModel.hpp"

#include "NumberData.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MultiplicationModel : public MathOperationDataModel
{
public:

  virtual
  ~MultiplicationModel() {}

public:

  QString
  caption() const override
  { return QString("Multiplication"); }

  static QString
  name()
  { return QString("Multiplication"); }

public:

  void
  save(Properties &p) const override
  {
    p.put("model_name", MultiplicationModel::name());
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
      _result = std::make_shared<NumberData>(n1->number() *
                                             n2->number());
    }
    else
    {
      _result.reset();
    }

    emit dataUpdated(outPortIndex);
  }
};
