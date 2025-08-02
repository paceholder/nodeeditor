#pragma once
#include "UIBufferBase.hpp"
class Scalar_Boolean_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(bool Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("boolean Checkbox Fixed Buffer"); }


private:
    bool _value;
};