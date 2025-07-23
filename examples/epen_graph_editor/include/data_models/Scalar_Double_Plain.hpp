#pragma once
#include "Scalar_Double_Base.hpp"
class Scalar_Double_Plain : public Scalar_Double_Base
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("double Plain Number Input Buffer"); }
};