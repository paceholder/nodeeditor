#pragma once
#include "UIBufferBase.hpp"
class Scalar_Boolean_Checkbox : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(bool Default_Value MEMBER _defaultValue NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("boolean Checkbox Input Buffer"); }


private:
    bool _defaultValue;
};