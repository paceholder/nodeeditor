#pragma once
#include "Scalar_UnsignedInt_Base.hpp"
class Scalar_UnsignedInt_Slider : public Scalar_UnsignedInt_Base
{
    Q_OBJECT
public:
    QString caption() const override { return QStringLiteral("unsigned int Slider Input Buffer"); }
};