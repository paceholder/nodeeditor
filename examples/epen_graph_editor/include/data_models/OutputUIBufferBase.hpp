#pragma once
#include "UIBufferBase.hpp"
class OutputUIBufferBase : public UIBufferBase
{
    Q_OBJECT
public:
    virtual ~OutputUIBufferBase() {}

    bool portCaptionVisible(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType);
        Q_UNUSED(portIndex);
        return true;
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        return BUFFER_DATA_TYPE;
    }

    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        switch (portType) {
        case PortType::Out:
            return QStringLiteral("Buffer");

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
};