#include "RoiRectModel.hpp"
#include "ClutterFilterData.hpp"
#include "RoiData.hpp"
#include "RoiRectWidget.hpp"
#include <QJsonValue>
#include <QJsonValueRef>
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeStyle;

unsigned int RoiRectModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 1;

    return result;
}

NodeDataType RoiRectModel::dataType(PortType portType, PortIndex) const
{
    if (portType == PortType::In) {
        return ClutterFilterData().type();
    } else {
        return RoiData().type();
    }
}

std::shared_ptr<NodeData> RoiRectModel::outData(PortIndex)
{
    return nullptr;
}

void RoiRectModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {}

QWidget *RoiRectModel::embeddedWidget()
{
    if (_widget) {
        return _widget;
    }

    _widget = new RoiRectWidget(_x1, _x2, _y1, _y2, _z1, _z2, nullptr);

    return _widget;
}

QJsonObject RoiRectModel::save() const
{
    QJsonObject retval = NodeDelegateModel::save();
    retval["x1"] = _x1;
    retval["x2"] = _x2;
    retval["y1"] = _y1;
    retval["y2"] = _y2;
    retval["z1"] = _z1;
    retval["z2"] = _z2;
    return retval;
}

void RoiRectModel::load(QJsonObject const &object)
{
    NodeDelegateModel::load(object);
    _x1 = object["x1"].toDouble();
    _x2 = object["x2"].toDouble();
    _y1 = object["y1"].toDouble();
    _y2 = object["y2"].toDouble();
    _z1 = object["z1"].toDouble();
    _z2 = object["z2"].toDouble();
}
