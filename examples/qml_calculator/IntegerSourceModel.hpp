#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtCore/QObject>
#include "IntegerData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class IntegerSourceModel : public NodeDelegateModel
{
    Q_OBJECT
    Q_PROPERTY(int number READ number WRITE setNumber NOTIFY numberChanged)

public:
    IntegerSourceModel()
        : _number(0)
    {}

    QString caption() const override { return QStringLiteral("Integer"); }
    QString name() const override { return QStringLiteral("IntegerSource"); }

    unsigned int nPorts(PortType portType) const override
    {
        return (portType == PortType::Out) ? 1 : 0;
    }

    NodeDataType dataType(PortType, PortIndex) const override
    {
        return IntegerData{}.type();
    }

    void setInData(std::shared_ptr<NodeData>, PortIndex) override {}

    std::shared_ptr<NodeData> outData(PortIndex) override
    {
        return std::make_shared<IntegerData>(_number);
    }

    QWidget *embeddedWidget() override { return nullptr; }

    int number() const { return _number; }

    void setNumber(int n)
    {
        if (_number != n) {
            _number = n;
            Q_EMIT numberChanged();
            Q_EMIT dataUpdated(0);
        }
    }

Q_SIGNALS:
    void numberChanged();

private:
    int _number;
};
