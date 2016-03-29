#pragma once

#include <QtCore/QObject>

#include <nodes/NodeData>
#include <nodes/NodeDataModel>

class MyNodeData : public NodeData
{

public:

  bool sameType(NodeData const &nodeData) const override
  {
    return this->type() == nodeData.type();
  }

  QString type() const override
  { return "MyNodeData"; }
};

// simplest stupid model
class NaiveDataModel : public NodeDataModel
{
  Q_OBJECT
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

  void computingStarted() override {  }
  void computingFinished() override {  }
};

