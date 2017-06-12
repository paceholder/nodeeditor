#pragma once

#include <QtCore/QObject>

#include <nodes/NodeDataModel>

#include <iostream>

class ExpressionRangeData;

class QWidget;
class QLabel;

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ExpressionDisplayModel
  : public NodeDataModel
{
  Q_OBJECT

public:
  ExpressionDisplayModel();

  virtual
  ~ExpressionDisplayModel() {}

public:

  QString
  caption() const override
  { return QStringLiteral("Expression Display"); }

  bool
  captionVisible() const override
  { return true; }

  QString
  name() const override
  { return QStringLiteral("Expression Display"); }

  std::unique_ptr<NodeDataModel>
  clone() const override
  { return std::make_unique<ExpressionDisplayModel>(); }

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
    setInData(std::shared_ptr<NodeData>, PortIndex) override;

  QWidget *
  embeddedWidget() override;

private:

  QString
  convertRangeToText(std::vector<double> const &range) const;

private:

  std::shared_ptr<ExpressionRangeData> _expression;

  QWidget * _widget;

  QLabel * _variableLabel;
  QLabel * _rangeLabel;
};
