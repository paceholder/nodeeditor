#pragma once
#include "CallbackManagedImageBase.hpp"

class InputCallbackManagedImage : public CallbackManagedImageBase
{
    Q_OBJECT
public:
    virtual ~InputCallbackManagedImage() {}

public:
    QString caption() const override { return QStringLiteral("Input Callback Managed Image"); }

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