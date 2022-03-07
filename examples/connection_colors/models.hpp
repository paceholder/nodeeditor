#pragma once

#include <QtCore/QObject>

#include <nodes/NodeData>
#include <nodes/NodeDataModel>

#include <memory>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDataModel;
using QtNodes::PortType;
using QtNodes::PortIndex;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class MyNodeData : public NodeData
{
public:

  NodeDataType
  type() const override
  {
    return NodeDataType {"MyNodeData",
                         "My Node Data"};
  }
};

class SimpleNodeData : public NodeData
{
public:

  NodeDataType
  type() const override
  {
    return NodeDataType {"SimpleData",
                         "Simple Data"};
  }
};

//------------------------------------------------------------------------------

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NaiveDataModel : public NodeDataModel
{
  Q_OBJECT

public:

  virtual
  ~NaiveDataModel() {}

public:

  QString
  caption() const override
  {
    return QString("Naive Data Model");
  }

  QString
  name() const override
  { return QString("NaiveDataModel"); }

public:

  unsigned int
  nPorts(PortType portType) const override
  {
    unsigned int result = 1;

    switch (portType)
    {
      case PortType::In:
        result = 2;
        break;

      case PortType::Out:
        result = 2;
        break;
      case PortType::None:
        break;
    }

    return result;
  }

  NodeDataType
  dataType(PortType portType,
           PortIndex portIndex) const override
  {
    switch (portType)
    {
      case PortType::In:
        switch (portIndex)
        {
          case 0:
            return MyNodeData().type();
          case 1:
            return SimpleNodeData().type();
        }
        break;

      case PortType::Out:
        switch (portIndex)
        {
          case 0:
            return MyNodeData().type();
          case 1:
            return SimpleNodeData().type();
        }
        break;

      case PortType::None:
        break;
    }
    // FIXME: control may reach end of non-void function [-Wreturn-type]
    return NodeDataType();
  }

  std::shared_ptr<NodeData>
  outData(PortIndex port) override
  {
    if (port < 1)
      return std::make_shared<MyNodeData>();

    return std::make_shared<SimpleNodeData>();
  }

  void
  setInData(std::shared_ptr<NodeData>, int) override
  {
    //
  }

  QWidget *
  embeddedWidget() override { return nullptr; }
};
