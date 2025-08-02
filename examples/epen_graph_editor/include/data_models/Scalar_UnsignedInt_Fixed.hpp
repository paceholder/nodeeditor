#pragma once
#include "UIBufferBase.hpp"
class Scalar_UnsignedInt_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(uint Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("unsigned int Fixed Buffer"); }

private:
    uint _value;
};