#include "data_models/OperationDataModel.hpp"

#include "VideoData.hpp"

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

void OperationDataModel::setupProperties(QtTreePropertyBrowser *variantEditor,
                                         QtVariantPropertyManager *variantManager)
{
    QtVariantProperty *item = variantManager->addProperty(QMetaType::QString, QLatin1String("Name"));
    item->setValue(_name);
    variantEditor->addProperty(item);
}
