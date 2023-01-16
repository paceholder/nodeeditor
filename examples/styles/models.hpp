#pragma once

#include <QtCore/QObject>

#include <QtNodes/NodeDelegateModel>

#include <memory>

using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

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
            createPort(PortType::In, "MyNodeData", "My Node Data");
            createPort(PortType::Out, "MyNodeData", "My Node Data", QtNodes::ConnectionPolicy::Many);
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
    void setInData(QVariant const, PortIndex const) override {}

    QWidget *embeddedWidget() override { return nullptr; }
};
