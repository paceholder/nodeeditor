#pragma once
#include "OutputUIBufferBase.hpp"
class Array_UnsignedInt_Fixed : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QString Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("Unsigned Int Array Fixed Buffer"); }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        return BUFFER_DATA_TYPE;
    }

private:
    QString _value;
};