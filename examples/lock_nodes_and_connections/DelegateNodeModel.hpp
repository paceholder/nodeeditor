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
    NodeDataType type() const override { return NodeDataType{"SimpleData", "Simple Data"}; }
};

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class SimpleDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    QString caption() const override { return QString("Simple Data Model"); }

    QString name() const override { return QString("SimpleDataModel"); }

public:
    unsigned int nPorts(PortType const portType) const override { return 2; }

    NodeDataType dataType(PortType const portType, PortIndex const portIndex) const override
    {
        return SimpleNodeData().type();
    }

    std::shared_ptr<NodeData> outData(PortIndex const port) override
    {
        return std::make_shared<SimpleNodeData>();
    }

    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {}

    QWidget *embeddedWidget() override { return nullptr; }
};
