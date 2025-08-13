#pragma once
#include "CallbackManagedImageBase.hpp"

class OutputCallbackManagedImage : public CallbackManagedImageBase
{
    Q_OBJECT
public:
    virtual ~OutputCallbackManagedImage() {}

public:
    QString caption() const override { return QStringLiteral("Output Callback Managed Image"); }


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