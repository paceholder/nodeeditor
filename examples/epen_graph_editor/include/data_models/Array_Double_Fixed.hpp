#pragma once
#include "UIBufferBase.hpp"
class Array_Double_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("Double Array Fixed Buffer"); }

private:
};