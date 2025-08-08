#pragma once
#include "Scalar_Float_Base.hpp"
class Scalar_Float_Plain : public Scalar_Float_Base
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("float Plain Number Input Buffer"); }
};