#pragma once
#include "EnumUtils.hpp"
#include "OperationDataModel.hpp"
class UIDataModelBase : public OperationDataModel
{
    Q_OBJECT
public:
    enum class BufferTypes { Scalar, Array };

    Q_ENUM(BufferTypes)

    virtual ~UIDataModelBase() {}

    void setupProperties(QtVariantPropertyManager *manager, QtTreePropertyBrowser *browser) override
    {
        OperationDataModel::setupProperties(manager, browser);
        _bufferTypeItem = manager->addProperty(QtVariantPropertyManager::enumTypeId(),
                                               "Buffer Type");
        QStringList bufferTypes = EnumUtils::enumToStringList<BufferTypes>();
        _bufferTypeItem->setAttribute(QLatin1String("enumNames"), bufferTypes);
        browser->addProperty(_bufferTypeItem);

        _bufferTypeItem->setValue(_currentBufferType);
    }

    void deselected(QtVariantPropertyManager *manager, QtTreePropertyBrowser *browser) override
    {
        OperationDataModel::deselected(manager, browser);
        delete _bufferTypeItem;
    }

    bool valueChanged(QString propertyName, const QVariant &val) override
    {
        if (OperationDataModel::valueChanged(propertyName, val))
            return true;
        if (propertyName == "Buffer Type") {
            _currentBufferType = val.toInt();
            return true;
        }
        return false;
    }

private:
    QtVariantProperty *_bufferTypeItem;
    int _currentBufferType = 0;
};