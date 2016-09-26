#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/NodeDataModel>

#include <iostream>

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NumberDisplayDataModel : public NodeDataModel
{
  Q_OBJECT

public:
  NumberDisplayDataModel();

  virtual
  ~NumberDisplayDataModel() {}

public:

  QString
  caption() const override
  { return QString("Result"); }

  bool
  captionVisible() const override
  { return false; }

  static QString
  name()
  { return QString("Result"); }

public:

  void
  save(Properties &p) const override
  {
    p.put("model_name", NumberDisplayDataModel::name());
  }

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType,
           PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData> data, int) override;

  QWidget *
  embeddedWidget() override { return _label; }

private:

  QLabel * _label;
};
