#pragma once
#include "UIBufferBase.hpp"
class Array_Double_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QString Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("Double Array Fixed Buffer"); }

private:
    QString _value;
};