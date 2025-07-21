#pragma once
#include "UIBufferBase.hpp"
class Array_Int_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("Int Array Fixed Buffer"); }

private:
};