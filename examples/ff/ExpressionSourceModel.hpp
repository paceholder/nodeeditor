#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>

#include <nodes/NodeDataModel>

#include <iostream>

class ExpressionData;

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::NodeValidationState;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class ExpressionSourceModel
  : public NodeDataModel
{
  Q_OBJECT

public:
  ExpressionSourceModel();

  virtual
  ~ExpressionSourceModel() {}

public:

  QString
  caption() const override
  { return QStringLiteral("Expression Source"); }

  bool
  captionVisible() const override
  { return false; }

  QString
  name() const override
  { return QStringLiteral("Expression Source"); }

  std::unique_ptr<NodeDataModel>
  clone() const override
  { return std::make_unique<ExpressionSourceModel>(); }

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
  setInData(std::shared_ptr<NodeData>, int) override
  { }

  QWidget *
  embeddedWidget() override { return _lineEdit; }

private slots:

  void
  onTextEdited(QString const &string);

private:

  std::shared_ptr<ExpressionData> _expression;

  QLineEdit * _lineEdit;
};
