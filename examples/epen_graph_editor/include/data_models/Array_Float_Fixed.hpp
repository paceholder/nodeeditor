#pragma once
#include "UIBufferBase.hpp"
class Array_Float_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("Float Array Fixed Buffer"); }

private:
};