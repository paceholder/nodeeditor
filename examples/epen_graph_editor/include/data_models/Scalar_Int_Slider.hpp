#pragma once
#include "Scalar_Int_Base.hpp"
class Scalar_Int_Slider : public Scalar_Int_Base
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("int Slider Input Buffer"); }
};