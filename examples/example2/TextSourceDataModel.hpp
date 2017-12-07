#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>

#include "TextData.hpp"

#include <nodes/NodeDataModel>

#include <iostream>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextSourceDataModel : public NodeDataModel
{
  Q_OBJECT

public:
  TextSourceDataModel();

  virtual
  ~TextSourceDataModel() {}

public:

  QString
  caption() const override
  { return QString("Text Source"); }

  bool
  captionVisible() const override { return false; }

  QString
  name() const override
  { return QString("TextSourceDataModel"); }


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

  QLineEdit * _lineEdit;
};
