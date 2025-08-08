#pragma once
#include "OutputUIBufferBase.hpp"
class Scalar_Boolean_Fixed : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(bool Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("boolean Checkbox Fixed Buffer"); }


private:
    bool _value;
};