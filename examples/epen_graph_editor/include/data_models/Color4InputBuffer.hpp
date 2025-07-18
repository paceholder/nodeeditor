#pragma once
#include "UIDataModelBase.hpp"

class Color4InputBuffer : public UIDataModelBase
{
public:
    virtual ~Color4InputBuffer() {}

public:
    QString caption() const override { return QStringLiteral("Color4 Input Buffer"); }

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

    QString name() const override { return QStringLiteral("Color4Buffer"); }

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

        _defaultValueItem = manager->addProperty(QVariant::Color, "Default Value");
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
            if (val.canConvert<QColor>()) {
                _defaultValue = val.value<QColor>();
            }
            return true;
        }
        return false;
    }

private:
    QtVariantProperty *_defaultValueItem;

    QColor _defaultValue{255, 255, 255, 255};
};