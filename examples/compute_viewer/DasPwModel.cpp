#include "DasPwModel.hpp"
#include "DasPwData.hpp"
#include "DasPwWidget.hpp"
#include "DecimationData.hpp"
#include <QJsonValue>
#include <QJsonValueRef>
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeStyle;

unsigned int DasPwModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 1;

    return result;
}

NodeDataType DasPwModel::dataType(PortType portType, PortIndex) const
{
    if (portType == PortType::In) {
        return DecimationData().type();
    } else {
        return DasPwData().type();
    }
}

std::shared_ptr<NodeData> DasPwModel::outData(PortIndex)
{
    return nullptr;
}

void DasPwModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {}

QWidget *DasPwModel::embeddedWidget()
{
    if (_widget) {
        return _widget;
    }

    _widget = new DasPwWidget(_f, _c0, nullptr);

    return _widget;
}

QJsonObject DasPwModel::save() const
{
    QJsonObject retval = NodeDelegateModel::save();
    retval["F"] = _f;
    retval["c0"] = _c0;
    return retval;
}

void DasPwModel::load(QJsonObject const &object)
{
    NodeDelegateModel::load(object);
    _f = object["F"].toDouble();
    _c0 = object["c0"].toDouble();
}
