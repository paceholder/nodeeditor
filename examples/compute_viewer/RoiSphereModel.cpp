#include "RoiSphereModel.hpp"
#include "ClutterFilterData.hpp"
#include "RoiData.hpp"
#include "RoiSphereWidget.hpp"
#include <QJsonValue>
#include <QJsonValueRef>
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeStyle;

unsigned int RoiSphereModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 1;

    return result;
}

NodeDataType RoiSphereModel::dataType(PortType portType, PortIndex) const
{
    if (portType == PortType::In) {
        return ClutterFilterData().type();
    } else {
        return RoiData().type();
    }
}

std::shared_ptr<NodeData> RoiSphereModel::outData(PortIndex)
{
    return nullptr;
}

void RoiSphereModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {}

QWidget *RoiSphereModel::embeddedWidget()
{
    if (_widget) {
        return _widget;
    }

    _widget = new RoiSphereWidget(_x, _y, _z, _r, nullptr);

    return nullptr;
}

QJsonObject RoiSphereModel::save() const
{
    QJsonObject retval = NodeDelegateModel::save();
    retval["x"] = _x;
    retval["y"] = _y;
    retval["z"] = _z;
    retval["r"] = _r;
    return retval;
}

void RoiSphereModel::load(QJsonObject const &object)
{
    NodeDelegateModel::load(object);
    _x = object["x"].toDouble();
    _y = object["y"].toDouble();
    _z = object["z"].toDouble();
    _r = object["r"].toDouble();
}
