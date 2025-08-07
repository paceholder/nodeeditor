#pragma once
#include "OperationDataModel.hpp"

class InputCallbackManagedImage : public OperationDataModel
{
    Q_OBJECT
public:
    virtual ~InputCallbackManagedImage() {}

public:
    QString caption() const override { return QStringLiteral("Input Callback Managed Image"); }

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
            return QStringLiteral("Image");

        default:
            break;
        }
        return QString();
    }

    unsigned int nPorts(PortType portType) const override
    {
        unsigned int result;

        if (portType == PortType::In)
            result = 0;
        else
            result = 1;

        return result;
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        return IMAGE_DATA_TYPE;
    }
};