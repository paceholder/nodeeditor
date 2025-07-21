#pragma once
#include "UIBufferBase.hpp"
class Scalar_Int_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(int Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("int Fixed Buffer"); }

private:
    int _value;
};