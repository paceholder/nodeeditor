#pragma once
#include "UIBufferBase.hpp"
class Array_Int_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QString Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("Int Array Fixed Buffer"); }

private:
    QString _value;
};