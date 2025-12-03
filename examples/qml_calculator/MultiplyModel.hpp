#pragma once

#include <QtNodes/NodeDelegateModel>
#include "DecimalData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class MultiplyModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    QString caption() const override { return QStringLiteral("Multiply"); }
    QString name() const override { return QStringLiteral("Multiply"); }

    unsigned int nPorts(PortType portType) const override
    {
        return (portType == PortType::In) ? 2 : 1;
    }

    NodeDataType dataType(PortType, PortIndex) const override
    {
        return DecimalData{}.type();
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override
    {
        auto numberData = std::dynamic_pointer_cast<DecimalData>(data);
        if (portIndex == 0) {
            _number1 = numberData;
        } else {
            _number2 = numberData;
        }
        compute();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override { return _result; }

    QWidget *embeddedWidget() override { return nullptr; }

private:
    void compute()
    {
        if (_number1 && _number2) {
            _result = std::make_shared<DecimalData>(_number1->number() * _number2->number());
        } else {
            _result.reset();
        }
        Q_EMIT dataUpdated(0);
    }

    std::shared_ptr<DecimalData> _number1;
    std::shared_ptr<DecimalData> _number2;
    std::shared_ptr<DecimalData> _result;
};
