#pragma once
#include "UIBufferBase.hpp"
class Scalar_Float_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(float Value MEMBER _value NOTIFY propertyChanged)
    QString caption() const override { return QStringLiteral("float Fixed Buffer"); }

private:
    float _value;
};