#pragma once
#include "UIBufferBase.hpp"
class Scalar_UnsignedInt_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(int Value MEMBER _value WRITE setValue NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("unsigned int Fixed Buffer"); }
    void setValue(int v)
    {
        if (v < 0)
            v = 0;
        _value = v;
    }

private:
    uint _value;
};