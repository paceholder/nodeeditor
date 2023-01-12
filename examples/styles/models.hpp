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

/// The class can potentially incapsulate any user data which need to
/// be transferred within the Node Editor graph
class MyNodeData : public NodeData
{
public:
    NodeDataType type() const override { return "MyNodeData"; }
};

//------------------------------------------------------------------------------

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MyDataModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    ~MyDataModel() = default;

public:
    void init() override
    {
        for (int i = 0; i < 3; ++i) {
            createPort(PortType::In, std::make_shared<MyNodeData>(), "My Node Data");
            createPort(PortType::Out,
                       std::make_shared<MyNodeData>(),
                       "My Node Data",
                       QtNodes::ConnectionPolicy::Many);
        }
    }

    QString caption() const override { return QString("My Data Model"); }

    QString name() const override { return QString("MyDataModel"); }

public:
    QJsonObject save() const override
    {
        QJsonObject modelJson;

        modelJson["name"] = name();

        return modelJson;
    }

public:
    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {}

    QWidget *embeddedWidget() override { return nullptr; }
};
