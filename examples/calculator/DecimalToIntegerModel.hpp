#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>

#include <nodes/NodeDataModel>

#include <iostream>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;

class DecimalData;
class IntegerData;

class DecimalToIntegerModel
  : public NodeDataModel
{
  Q_OBJECT

public:
  DecimalToIntegerModel() = default;

  virtual
  ~DecimalToIntegerModel() = default;

public:

  QString
  caption() const override
  { return QStringLiteral("Decimal to integer"); }

  bool
  captionVisible() const override
  { return false; }

  QString
  name() const override
  { return QStringLiteral("DecimalToInteger"); }

  std::unique_ptr<NodeDataModel>
  clone() const override
  { return std::make_unique<DecimalToIntegerModel>(); }

public:

  QJsonObject
  save() const override;

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData>, int) override;

  QWidget *
  embeddedWidget() override { return nullptr; }

private:

  std::shared_ptr<DecimalData> _decimal;
  std::shared_ptr<IntegerData> _integer;
};
