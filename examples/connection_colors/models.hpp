#pragma once

#include <QtCore/QObject>

#include <QtNodes/NodeData>
#include <QtNodes/NodeDelegateModel>

#include <memory>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The class can potentially incapsulate any user data which
/// need to be transferred within the Node Editor graph
class MyNodeData : public NodeData
{
public:
    NodeDataType type() const override { return "MyNodeData"; }
};

class SimpleNodeData : public NodeData
{
public:
    NodeDataType type() const override { return "SimpleData"; }
};

//------------------------------------------------------------------------------

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NaiveDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    virtual ~NaiveDataModel() {}

public:
    void init() override
    {
        createPort(PortType::In, std::make_shared<MyNodeData>(), "My Node Data");
        createPort(PortType::In, std::make_shared<SimpleNodeData>(), "Simple Data");

        createPort(PortType::Out,
                   std::make_shared<MyNodeData>(),
                   "My Node Data",
                   QtNodes::ConnectionPolicy::Many);
        createPort(PortType::Out,
                   std::make_shared<SimpleNodeData>(),
                   "Simple Data",
                   QtNodes::ConnectionPolicy::Many);
    }

    QString caption() const override { return QString("Naive Data Model"); }

    QString name() const override { return QString("NaiveDataModel"); }

public:
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override
    {
        //
    }

    QWidget *embeddedWidget() override { return nullptr; }
};
