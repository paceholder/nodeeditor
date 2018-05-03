#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include "TextData.hpp"

#include <nodes/NodeDataModel>

#include <iostream>

using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::NodeData;
using QtNodes::NodeDataModel;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextDisplayDataModel : public NodeDataModel
{
  Q_OBJECT

public:
  TextDisplayDataModel();

  virtual
  ~TextDisplayDataModel() {}

public:

  QString
  caption() const override
  { return QString("Text Display"); }

  bool
  captionVisible() const override { return false; }

  static QString
  Name()
  { return QString("TextDisplayDataModel"); }

  QString
  name() const override
  { return TextDisplayDataModel::Name(); }

public:

  unsigned int
  nPorts(PortType portType) const override;

  NodeDataType
  dataType(PortType portType, PortIndex portIndex) const override;

  std::shared_ptr<NodeData>
  outData(PortIndex port) override;

  void
  setInData(std::shared_ptr<NodeData> data, int) override
  {
    auto textData = std::dynamic_pointer_cast<TextData>(data);

    if (textData)
    {
      _label->setText(textData->text());
    }
    else
    {
      _label->clear();
    }

    _label->adjustSize();
  }

  QWidget *
  embeddedWidget() override { return _label; }

private:

  QLabel * _label;
};
