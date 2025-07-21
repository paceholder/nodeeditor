#pragma once
#include "OperationDataModel.hpp"

class Process : public OperationDataModel
{
    Q_OBJECT
public:
    virtual ~Process() {}

public:
    QString caption() const override { return QStringLiteral("Process"); }

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

    unsigned int nPorts(PortType portType) const override
    {
        unsigned int result;

        if (portType == PortType::In)
            result = 1;
        else
            result = 1;

        return result;
    }
};