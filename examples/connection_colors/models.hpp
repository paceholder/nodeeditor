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
        createPort(PortType::Out, "MyNodeData", "My Node Data", QtNodes::ConnectionPolicy::Many);
        createPort(PortType::Out, "SimpleData", "Simple Data", QtNodes::ConnectionPolicy::Many);

        createPort(PortType::In, "MyNodeData", "My Node Data");
        createPort(PortType::In, "SimpleData", "Simple Data");
    }

    QString caption() const override { return QString("Naive Data Model"); }

    QString name() const override { return QString("NaiveDataModel"); }

public:
    void setInData(QVariant const, PortIndex const) override {}

    QWidget *embeddedWidget() override { return nullptr; }
};
