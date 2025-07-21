#pragma once
#include "OperationDataModel.hpp"

class Color4InputBuffer : public OperationDataModel
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


private:
    QtVariantProperty *_defaultValueItem;

    QColor _defaultValue{255, 255, 255, 255};
};