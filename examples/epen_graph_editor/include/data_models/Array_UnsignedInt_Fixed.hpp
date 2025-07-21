#pragma once
#include "UIBufferBase.hpp"
class Array_UnsignedInt_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("Unsigned Int Array Fixed Buffer"); }

private:
};