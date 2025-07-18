#pragma once
#include "UIDataModelBase.hpp"
class NumberInputBufferBase : public UIDataModelBase
{
    Q_OBJECT
public:
    enum class VarTypes { Float, Int, Float4 };

    Q_ENUM(VarTypes)

    virtual ~NumberInputBufferBase() {}

    void setupProperties(QtVariantPropertyManager *manager, QtTreePropertyBrowser *browser) override
    {
        UIDataModelBase::setupProperties(manager, browser);

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

        _varTypeItem->setValue(_currentVarType);
        _defaultValueItem->setValue(_defaultValue);
        _minValueItem->setValue(_minValue);
        _maxValueItem->setValue(_maxValue);
    }

    void deselected(QtVariantPropertyManager *manager, QtTreePropertyBrowser *browser) override
    {
        UIDataModelBase::deselected(manager, browser);
        delete _varTypeItem;
        delete _defaultValueItem;
        delete _minValueItem;
        delete _maxValueItem;
    }

    bool valueChanged(QString propertyName, const QVariant &val) override
    {
        if (UIDataModelBase::valueChanged(propertyName, val))
            return true;
        if (propertyName == "Type") {
            _currentVarType = val.toInt();
            return true;
        } else if (propertyName == "Default Value") {
            _defaultValue = val.toDouble();
            return true;
        } else if (propertyName == "Min") {
            _minValue = val.toDouble();
            return true;
        } else if (propertyName == "Max") {
            _maxValue = val.toDouble();
            return true;
        }
        return false;
    }

private:
    QtVariantProperty *_varTypeItem;
    QtVariantProperty *_defaultValueItem;
    QtVariantProperty *_minValueItem;
    QtVariantProperty *_maxValueItem;
    int _currentVarType = 0;
    float _defaultValue = 0;
    float _minValue = 0;
    float _maxValue = 0;
};