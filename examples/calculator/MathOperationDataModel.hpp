#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/NodeDataModel>

#include <iostream>

class NumberData;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MathOperationDataModel : public NodeDataModel
{
  Q_OBJECT

public:

  virtual ~MathOperationDataModel() {}

public:

  unsigned int nPorts(PortType portType) const override;

  NodeDataType dataType(PortType portType,
                        PortIndex portIndex) const override;

  std::shared_ptr<NodeData> outData(PortIndex port) override;

  void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;

  QWidget * embeddedWidget() override { return nullptr; }

protected:

  virtual void compute() = 0;

protected:

  std::weak_ptr<NumberData> _number1;
  std::weak_ptr<NumberData> _number2;

  std::shared_ptr<NumberData> _result;
};
