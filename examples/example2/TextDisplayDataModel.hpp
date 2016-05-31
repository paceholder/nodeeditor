#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include "TextData.hpp"

#include <nodes/NodeDataModel>

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextDisplayDataModel : public NodeDataModel
{
  Q_OBJECT

public:
  TextDisplayDataModel();

  virtual ~TextDisplayDataModel() {}

public:

  unsigned int nSlots(PortType portType) const override;

  std::shared_ptr<NodeData>
  data(PortType, int slot) override;

  void setInputData(std::shared_ptr<NodeData>, int) override
  {
    //
  }

  QWidget * embeddedWidget() override { return _label; }

signals:

  void computingStarted() override;
  void computingFinished() override;

private:

  QLabel * _label;
};
