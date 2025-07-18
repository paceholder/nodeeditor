#pragma once
#include "UIDataModelBase.hpp"

class CheckboxInputBuffer : public UIDataModelBase
{
public:
    virtual ~CheckboxInputBuffer() {}

public:
    QString caption() const override { return QStringLiteral("Checkbox Input Buffer"); }

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

    QString name() const override { return QStringLiteral("CheckboxBuffer"); }

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
        UIDataModelBase::setupProperties(manager, browser);

        _defaultValueItem = manager->addProperty(QVariant::Bool, "Default Value");
        browser->addProperty(_defaultValueItem);

        _defaultValueItem->setValue(_defaultValue);
    }

    void deselected(QtVariantPropertyManager *manager, QtTreePropertyBrowser *browser) override
    {
        UIDataModelBase::deselected(manager, browser);
        delete _defaultValueItem;
    }

    bool valueChanged(QString propertyName, const QVariant &val) override
    {
        if (UIDataModelBase::valueChanged(propertyName, val))
            return true;
        if (propertyName == "Default Value") {
            _defaultValue = val.toBool();
            return true;
        }
        return false;
    }

private:
    QtVariantProperty *_defaultValueItem;

    bool _defaultValue = false;
};