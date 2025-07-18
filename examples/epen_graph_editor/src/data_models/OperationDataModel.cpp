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
    _nameItem = variantManager->addProperty(QVariant::String, QLatin1String("Name"));

    _nameItem->setValue(_name);
    /*conn = QObject::connect(variantManager,
                            &QtVariantPropertyManager::valueChanged,
                            [=](QtProperty *property, const QVariant &val) {
                                _name = val.toString();
                            });*/
    connect(variantManager,
            &QtVariantPropertyManager::valueChanged,
            this,
            &OperationDataModel::nameChanged);
    browser->addProperty(_nameItem);
}

void OperationDataModel::nameChanged(QtProperty *property, const QVariant &val)
{
    _name = val.toString();
}

void OperationDataModel::deselected(QtVariantPropertyManager *variantManager,
                                    QtTreePropertyBrowser *browser)
{
    disconnect(variantManager,
               &QtVariantPropertyManager::valueChanged,
               this,
               &OperationDataModel::nameChanged);
    delete _nameItem;
}