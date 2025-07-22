#pragma once
#include "UIBufferBase.hpp"
class Array_Float_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QString Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("Float Array Fixed Buffer"); }

private:
    QString _value;
};