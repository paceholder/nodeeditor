#pragma once
#include "OperationDataModel.hpp"

class OutputCallbackManagedImage : public OperationDataModel
{
    Q_OBJECT
public:
    virtual ~OutputCallbackManagedImage() {}

public:
    QString caption() const override { return QStringLiteral("Output Callback Managed Image"); }

    bool portCaptionVisible(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType);
        Q_UNUSED(portIndex);
        return true;
    }

    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        switch (portType) {
        case PortType::In:
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
            result = 1;
        else
            result = 0;

        return result;
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        return IMAGE_DATA_TYPE;
    }
};