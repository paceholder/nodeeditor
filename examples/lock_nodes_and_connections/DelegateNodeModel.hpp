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
class SimpleDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    void init() override
    {
        for (int i = 0; i < 2; ++i) {
            createPort(PortType::In, "SimpleData", "Simple Data");
            createPort(PortType::Out, "SimpleData", "Simple Data", QtNodes::ConnectionPolicy::Many);
        }
    }

    QString caption() const override { return QString("Simple Data Model"); }

    QString name() const override { return QString("SimpleDataModel"); }

public:
    void setInData(QVariant const, PortIndex const) override {}

    QWidget *embeddedWidget() override { return nullptr; }
};
