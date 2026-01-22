#pragma once

#include <QtNodes/NodeDelegateModel>
#include "DecimalData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class AdditionModel : public NodeDelegateModel
{
    Q_OBJECT
public:
    AdditionModel() {}

    QString caption() const override { return QStringLiteral("Addition"); }
    QString name() const override { return QStringLiteral("Addition"); }

    unsigned int nPorts(PortType portType) const override {
         if (portType == PortType::In) return 2;
         else return 1;
    }

    NodeDataType dataType(PortType, PortIndex) const override {
        return DecimalData().type();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override {
        return _result;
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override {
        auto numberData = std::dynamic_pointer_cast<DecimalData>(data);
        if (portIndex == 0) _number1 = numberData;
        else _number2 = numberData;

        compute();
    }

    QWidget *embeddedWidget() override { return nullptr; }

private:
    void compute() {
        if (_number1 && _number2) {
            _result = std::make_shared<DecimalData>(_number1->number() + _number2->number());
        } else {
            _result.reset();
        }
        Q_EMIT dataUpdated(0);
    }

    std::shared_ptr<DecimalData> _number1;
    std::shared_ptr<DecimalData> _number2;
    std::shared_ptr<DecimalData> _result;
};
