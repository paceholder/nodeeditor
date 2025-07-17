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

void OperationDataModel::setupProperties(QtTreePropertyBrowser *variantEditor,
                                         QtVariantPropertyManager *variantManager)
{
    QtIntPropertyManager *intManager = new QtIntPropertyManager(this);
    QObject::connect(intManager,
                     &QtIntPropertyManager::valueChanged,
                     [=](QtProperty *property, int val) { qDebug() << "VVV" << val; });
    QtProperty *item = intManager->addProperty(QLatin1String("Name"));
    intManager->setRange(item, -100, 100);
    //item->setValue(0);
    variantEditor->addProperty(item);
}

void OperationDataModel::deselected() {}