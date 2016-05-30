#pragma once

#include <QtCore/QObject>

#include <QtWidgets/QLineEdit>

#include <nodes/NodeData>
#include <nodes/NodeDataModel>


/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class MyNodeData : public NodeData
{
public:

  QString type() const override
  { return "MyNodeData"; }

  QString name() const override
  { return "My Node Data"; }
};

//------------------------------------------------------------------------------

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class DataModelWithWidget : public NodeDataModel
{
  Q_OBJECT

public:
  DataModelWithWidget();

  virtual ~DataModelWithWidget() {}

public:

  unsigned int nSlots(PortType portType) const override
  {
    unsigned int result = 1;

    switch (portType)
    {
      case PortType::IN:
        result = 2;
        break;

      case PortType::OUT:
        result = 3;

      default:
        break;
    }

    return result;
  }

  std::shared_ptr<NodeData>
  data(PortType, int slot) override
  {
    return std::make_shared<MyNodeData>();
  }

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
