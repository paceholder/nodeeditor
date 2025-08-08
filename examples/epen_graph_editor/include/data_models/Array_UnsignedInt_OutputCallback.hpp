#pragma once
#include "OutputUIBufferBase.hpp"

class Array_UnsignedInt_OutputCallback : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QString Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override
    {
        return QStringLiteral("Output Unsigned Integer Array Callback Managed Buffer");
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        return BUFFER_DATA_TYPE;
    }

private:
    QString _value;
};