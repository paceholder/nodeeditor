#include "data_models/OperationDataModel.hpp"
#include "VideoData.hpp"
#include "qtpropertymanager.h"

OperationDataModel::OperationDataModel()
{
    _name = "NewNode001";
}

unsigned int OperationDataModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 1;

    return result;
}

NodeDataType OperationDataModel::dataType(PortType, PortIndex) const
{
    return VideoData().type();
}

std::shared_ptr<NodeData> OperationDataModel::outData(PortIndex)
{
    return std::static_pointer_cast<NodeData>(_result);
}

void OperationDataModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
    auto numberData = std::dynamic_pointer_cast<VideoData>(data);

    if (!data) {
        Q_EMIT dataInvalidated(0);
    }

    if (portIndex == 0) {
        _number1 = numberData;
    } else {
        _number2 = numberData;
    }
}

void OperationDataModel::setupProperties(QtVariantPropertyManager *variantManager,
                                         QtTreePropertyBrowser *browser)
{
    _nameItem = variantManager->addProperty(QVariant::String, "Name");
    _nameItem->setValue(_name);
    browser->addProperty(_nameItem);
}

void OperationDataModel::deselected(QtVariantPropertyManager *variantManager,
                                    QtTreePropertyBrowser *browser)
{
    delete _nameItem;
}

bool OperationDataModel::valueChanged(QString propertyName, const QVariant &val)
{
    if (propertyName == "Name") {
        _name = val.toString();
        return true;
    }
    return false;
}

void OperationDataModel::setNodeName(QString name)
{
    _name = name;
}