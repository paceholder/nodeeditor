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
  { return QStringLiteral("Result"); }

  bool
  captionVisible() const override
  { return false; }

  QString
  name() const override
  { return QStringLiteral("Result"); }

  std::unique_ptr<NodeDataModel>
  clone() const override
  { return std::make_unique<NumberDisplayDataModel>(); }

public:

  void
  save(Properties &p) const override
  {
    p.put("model_name", name());
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

  NodeValidationState 
  validationState() const override;

  QString 
  validationMessage() const override;

private:

  NodeValidationState modelValidationState = NodeValidationState::Warning;
  QString modelValidationError = QStringLiteral("Missing or incorrect inputs");

  QLabel * _label;
};
