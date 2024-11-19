#include "CalculatorModel.hpp"
#include "AverageData.hpp"
#include "CalculatorData.hpp"
#include "CalculatorWidget.hpp"
#include <QJsonValue>
#include <QJsonValueRef>
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeStyle;

unsigned int CalculatorModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 2;
    else
        result = 1;

    return result;
}

NodeDataType CalculatorModel::dataType(PortType portType, PortIndex portIndex) const
{
    if (portType == PortType::In) {
        if (portIndex == 0) {
            return AverageX1Data().type();
        } else {
            return AverageX2Data().type();
        }
    } else {
        return CalculatorData().type();
    }
}

std::shared_ptr<NodeData> CalculatorModel::outData(PortIndex)
{
    return nullptr;
}

void CalculatorModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {}

QWidget *CalculatorModel::embeddedWidget()
{
    if (_widget) {
        return _widget;
    }

    _widget = new CalculatorWidget(_value, nullptr);

    return _widget;
}

QJsonObject CalculatorModel::save() const
{
    QJsonObject retval = NodeDelegateModel::save();
    retval["expression"] = _value;
    return retval;
}

void CalculatorModel::load(QJsonObject const &object)
{
    NodeDelegateModel::load(object);
    _value = object["expression"].toString();
}
