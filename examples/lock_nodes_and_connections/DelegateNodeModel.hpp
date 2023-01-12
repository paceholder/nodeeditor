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

class SimpleNodeData : public NodeData
{
public:
    NodeDataType type() const override { return "SimpleData"; }
};

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class SimpleDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    void init() override
    {
        for (int i = 0; i < 2; ++i) {
            createPort(PortType::In, std::make_shared<SimpleNodeData>(), "Simple Data");
            createPort(PortType::Out,
                       std::make_shared<SimpleNodeData>(),
                       "Simple Data",
                       QtNodes::ConnectionPolicy::Many);
        }
    }

    QString caption() const override { return QString("Simple Data Model"); }

    QString name() const override { return QString("SimpleDataModel"); }

public:
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {}

    QWidget *embeddedWidget() override { return nullptr; }
};
