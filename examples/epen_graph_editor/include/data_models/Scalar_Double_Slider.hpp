#pragma once
#include "Scalar_Double_Base.hpp"
class Scalar_Double_Slider : public Scalar_Double_Base
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("double Slider Input Buffer"); }
};