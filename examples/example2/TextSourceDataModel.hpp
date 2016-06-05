#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLineEdit>

#include "TextData.hpp"

#include <nodes/NodeDataModel>

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextSourceDataModel : public NodeDataModel
{
  Q_OBJECT

public:
  TextSourceDataModel();

  virtual ~TextSourceDataModel() {}

public:

  unsigned int nPorts(PortType portType) const override;

  std::shared_ptr<NodeData>
  data(PortType, int slot) override;

  void setInputData(std::shared_ptr<NodeData>, int) override
  {
    //
  }

  QWidget * embeddedWidget() override { return _lineEdit; }

signals:

  void computingStarted() override;
  void computingFinished() override;

private:

  QLineEdit * _lineEdit;
};
