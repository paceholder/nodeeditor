#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>

#include <nodes/NodeDataModel>

#include <iostream>

class IntegerData;

class ModuloModel
  : public NodeDataModel
{
  Q_OBJECT

public:
  ModuloModel();

  virtual
    ~ModuloModel() {}

public:

  QString
  caption() const override
  { return QString("Modulo"); }

  bool
  captionVisible() const override
  { return true; }

  bool
  portCaptionVisible(PortType portType, PortIndex portIndex) const override
  {
    return true;
  }

  QString
  portCaption(PortType portType, PortIndex portIndex) const override
  {
    switch (portType)
    {
    case PortType::In:
      if (portIndex == 0)
        return QString("Dividend");
      else if (portIndex == 1)
        return QString("Divisor");
      break;

    case PortType::Out:
      return QString("Result");

    default:
      break;
    }
    return QString("");
  }

  QString
  name() const override
  { return QString("Modulo"); }

  std::unique_ptr<NodeDataModel>
  clone() const override
  { return std::make_unique<ModuloModel>(); }

public:

  void
  save(Properties &p) const override;

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData>, int) override;

  QWidget * embeddedWidget() override { return nullptr; }

  NodeValidationState validationState() const override;

  QString validationMessage() const override;

private:

  std::weak_ptr<IntegerData> _number1;
  std::weak_ptr<IntegerData> _number2;

  std::shared_ptr<IntegerData> _result;

  NodeValidationState modelValidationState = NodeValidationState::Warning;
  QString modelValidationError = QString("Missing or incorrect inputs");
};
