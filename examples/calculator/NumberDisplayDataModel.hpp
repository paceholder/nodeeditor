#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <nodes/NodeDataModel>

#include <iostream>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NumberDisplayDataModel : public NodeDataModel
{
  Q_OBJECT

public:
  NumberDisplayDataModel();

  ~NumberDisplayDataModel() = default;

public:

  QString
  caption() const override
  { return QStringLiteral("Result"); }

  bool
  captionVisible() const override
  { return false; }

  QString
  name() const override
  { return QStringLiteral("Result"); }

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType  portType,
           PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData> data,
            PortIndex portIndex) override;

  QWidget *
  embeddedWidget() override { return _label; }

  QLabel * _label;
};
