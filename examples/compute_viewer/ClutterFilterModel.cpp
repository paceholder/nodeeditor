#include "ClutterFilterModel.hpp"
#include "ClutterFilterData.hpp"
#include "ClutterFilterWidget.hpp"
#include "DasPwData.hpp"
#include <QJsonValue>
#include <QJsonValueRef>
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeStyle;

unsigned int ClutterFilterModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 1;

    return result;
}

NodeDataType ClutterFilterModel::dataType(PortType portType, PortIndex) const
{
    if (portType == PortType::In) {
        return DasPwData().type();
    } else {
        return ClutterFilterData().type();
    }
}

std::shared_ptr<NodeData> ClutterFilterModel::outData(PortIndex)
{
    return nullptr;
}

void ClutterFilterModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {}

QWidget *ClutterFilterModel::embeddedWidget()
{
    if (_widget) {
        return _widget;
    }

    _widget = new ClutterFilterWidget(_value, nullptr);

    return _widget;
}

QJsonObject ClutterFilterModel::save() const
{
    QJsonObject retval = NodeDelegateModel::save();
    retval["value"] = _value;
    return retval;
}

void ClutterFilterModel::load(QJsonObject const &object)
{
    NodeDelegateModel::load(object);
    _value = object["value"].toDouble();
}
