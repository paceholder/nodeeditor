#pragma once

#include <QtCore/QObject>

#include <nodes/NodeData>
#include <nodes/NodeDataModel>


/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class MyNodeData : public NodeData
{
public:

  bool sameType(NodeData const &nodeData) const override
  {
    return this->type() == nodeData.type();
  }

  QString type() const override
  { return "MyNodeData"; }

  QString name() const override
  { return "Data"; }
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

  unsigned int nSlots(EndType) const override
  {
    return 3;
  }

  std::shared_ptr<NodeData>
  data(EndType, int) override
  { return std::make_shared<MyNodeData>(); }

  void setInputData(std::shared_ptr<NodeData>, int) override
  {
    //
  }

  QWidget * embeddedWidget() override { return nullptr; }

signals:

  void computingStarted() override;
  void computingFinished() override;
};
