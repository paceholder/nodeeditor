#pragma once
#include "UIBufferBase.hpp"
class Scalar_Double_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(double Value MEMBER _value NOTIFY propertyChanged)
    QString caption() const override { return QStringLiteral("double Fixed Buffer"); }

private:
    double _value;
};