#pragma once
#include "OperationDataModel.hpp"

class GpuInputBuffer : public OperationDataModel
{
public:
    virtual ~GpuInputBuffer() {}

public:
    QString caption() const override { return QStringLiteral("Gpu Input Buffer"); }

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

    QString name() const override { return QStringLiteral("InBuffer"); }

    unsigned int nPorts(PortType portType) const override
    {
        unsigned int result;

        if (portType == PortType::In)
            result = 0;
        else
            result = 1;

        return result;
    }
};