#pragma once
#include "EnumUtils.hpp"
#include "OperationDataModel.hpp"
class SliderInputBuffer : public OperationDataModel
{
    Q_OBJECT
public:
    enum class BufferTypes { Scalar, Array };

    enum class VarTypes { Float, Int, Float4 };

    Q_ENUM(BufferTypes)
    Q_ENUM(VarTypes)

    virtual ~SliderInputBuffer() {}

    QString caption() const override { return QStringLiteral("Slider Input Buffer"); }

    bool portCaptionVisible(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType);
        Q_UNUSED(portIndex);
        return true;
    }

    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        switch (portType) {
        case PortType::Out:
            return QStringLiteral("Video");

        default:
            break;
        }
        return QString();
    }

    QString name() const override { return QStringLiteral("SliderBuffer"); }

    unsigned int nPorts(PortType portType) const override
    {
        unsigned int result;

        if (portType == PortType::In)
            result = 0;
        else
            result = 1;

        return result;
    }

    void setupProperties(QtVariantPropertyManager *manager, QtTreePropertyBrowser *browser) override
    {
        OperationDataModel::setupProperties(manager, browser);
        _bufferTypeItem = manager->addProperty(QtVariantPropertyManager::enumTypeId(),
                                               "Buffer Type");
        QStringList bufferTypes = EnumUtils::enumToStringList<BufferTypes>();
        _bufferTypeItem->setAttribute(QLatin1String("enumNames"), bufferTypes);
        browser->addProperty(_bufferTypeItem);

        _varTypeItem = manager->addProperty(QtVariantPropertyManager::enumTypeId(), "Type");
        QStringList varTypes = EnumUtils::enumToStringList<VarTypes>();
        for (QString &item : varTypes) {
            item = item.toLower();
        }
        _varTypeItem->setAttribute(QLatin1String("enumNames"), varTypes);
        browser->addProperty(_varTypeItem);

        _defaultValueItem = manager->addProperty(QVariant::Double, "Default Value");
        browser->addProperty(_defaultValueItem);

        _minValueItem = manager->addProperty(QVariant::Double, "Min");
        browser->addProperty(_minValueItem);

        _maxValueItem = manager->addProperty(QVariant::Double, "Max");
        browser->addProperty(_maxValueItem);

        _bufferTypeItem->setValue(_currentBufferType);
        _varTypeItem->setValue(_currentVarType);
        _defaultValueItem->setValue(_defaultValue);
        _minValueItem->setValue(_minValue);
        _maxValueItem->setValue(_maxValue);
    }

    void deselected(QtVariantPropertyManager *manager, QtTreePropertyBrowser *browser) override
    {
        OperationDataModel::deselected(manager, browser);
        delete _bufferTypeItem;
        delete _varTypeItem;
        delete _defaultValueItem;
        delete _minValueItem;
        delete _maxValueItem;
    }

    bool valueChanged(QString propertyName, const QVariant &val) override
    {
        if (OperationDataModel::valueChanged(propertyName, val))
            return true;
        if (propertyName == "Buffer Type")
            _currentBufferType = val.toInt();
        else if (propertyName == "Type")
            _currentVarType = val.toInt();
        else if (propertyName == "Default Value")
            _defaultValue = val.toDouble();
        else if (propertyName == "Min")
            _minValue = val.toDouble();
        else if (propertyName == "Max")
            _maxValue = val.toDouble();
        return true;
    }

private:
    QtVariantProperty *_bufferTypeItem;
    QtVariantProperty *_varTypeItem;
    QtVariantProperty *_defaultValueItem;
    QtVariantProperty *_minValueItem;
    QtVariantProperty *_maxValueItem;
    int _currentBufferType = 0;
    int _currentVarType = 0;
    float _defaultValue = 0;
    float _minValue = 0;
    float _maxValue = 0;
};