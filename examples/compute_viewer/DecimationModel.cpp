#include "DecimationModel.hpp"
#include "DecimationData.hpp"
#include "DecimationWidget.hpp"
#include "InputData.hpp"
#include <QJsonValue>
#include <QJsonValueRef>
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeStyle;

unsigned int DecimationModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 1;

    return result;
}

NodeDataType DecimationModel::dataType(PortType portType, PortIndex) const
{
    if (portType == PortType::In) {
        return InputData().type();
    } else {
        return DecimationData().type();
    }
}

std::shared_ptr<NodeData> DecimationModel::outData(PortIndex)
{
    return nullptr;
}

void DecimationModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {}

QWidget *DecimationModel::embeddedWidget()
{
    if (_widget) {
        return _widget;
    }

    _widget = new DecimationWidget(_m, nullptr);

    return _widget;
}

QJsonObject DecimationModel::save() const
{
    QJsonObject retval = NodeDelegateModel::save();
    retval["M"] = _m;
    return retval;
}

void DecimationModel::load(QJsonObject const &object)
{
    NodeDelegateModel::load(object);
    _m = object["M"].toDouble();
}
