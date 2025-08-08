#pragma once
#include "UIBufferBase.hpp"
class InputUIBufferBase : public UIBufferBase
{
    Q_OBJECT
public:
    virtual ~InputUIBufferBase() {}

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
        case PortType::In:
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
            result = 1;
        else
            result = 0;

        return result;
    }
};