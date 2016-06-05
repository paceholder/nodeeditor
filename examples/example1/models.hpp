#pragma once

#include <QtCore/QObject>

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

class SimpleNodeData : public NodeData
{
public:

  QString type() const override
  { return "SimpleData"; }

  QString name() const override
  { return "Simple Data"; }
};

//------------------------------------------------------------------------------

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NaiveDataModel : public NodeDataModel
{
  Q_OBJECT

public:

  virtual ~NaiveDataModel() {}

public:

  unsigned int nPorts(PortType portType) const override
  {
    unsigned int result = 1;

    switch (portType)
    {
      case PortType::IN:
        result = 2;
        break;

      case PortType::OUT:
        result = 1;

      default:
        break;
    }

    return result;
  }

  std::shared_ptr<NodeData>
  data(PortType, int slot) override
  {
    if (slot < 1)
      return std::make_shared<MyNodeData>();

    return std::make_shared<SimpleNodeData>();
  }

  void setInputData(std::shared_ptr<NodeData>, int) override
  {
    //
  }

  QWidget * embeddedWidget() override { return nullptr; }

signals:

  void computingStarted() override;
  void computingFinished() override;
};
