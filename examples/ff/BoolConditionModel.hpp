#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QComboBox>

#include <nodes/NodeDataModel>

#include <iostream>

class ExpressionRangeData;

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class BoolConditionModel
  : public NodeDataModel
{
  Q_OBJECT

public:
  BoolConditionModel();

  virtual
  ~BoolConditionModel() {}

public:

  QString
  caption() const override
  { return QStringLiteral("Bool Condition"); }

  bool
  captionVisible() const override
  { return false; }

  QString
  name() const override
  { return QStringLiteral("Bool Condition"); }

  std::unique_ptr<NodeDataModel>
  clone() const override
  { return std::make_unique<BoolConditionModel>(); }

public:

  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &p) override;

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData> data, int) override;

  QWidget *
  embeddedWidget() override { return _comboBox; }

private slots:

  void
  onIndexChanged(QString const &string);

  void
  evaluate();

private:

  std::weak_ptr<ExpressionRangeData> _input1;
  std::weak_ptr<ExpressionRangeData> _input2;
  
  QString _boolCondition;

  std::shared_ptr<ExpressionRangeData> _expression;

  QComboBox * _comboBox;
};
