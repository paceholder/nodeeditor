#pragma once
#include "UIBufferBase.hpp"

class Array_Int_InputCallback : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QString Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("Input Integer Array Callback Managed Buffer"); }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        return BUFFER_DATA_TYPE;
    }

private:
    QString _value;
};