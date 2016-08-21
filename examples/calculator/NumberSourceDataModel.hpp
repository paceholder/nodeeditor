#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>

#include <nodes/NodeDataModel>

#include <iostream>

class NumberData;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NumberSourceDataModel : public NodeDataModel
{
  Q_OBJECT

public:
  NumberSourceDataModel();

  virtual ~NumberSourceDataModel() {}

public:

  unsigned int nPorts(PortType portType) const override;

  NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>outData(PortIndex port) override;

  void setInData(std::shared_ptr<NodeData>, int) override
  { }

  QWidget * embeddedWidget() override { return _lineEdit; }

private slots:

  void onTextEdited(QString const &string);

private:

  std::shared_ptr<NumberData> _number;

  QLineEdit * _lineEdit;
};
