#pragma once
#include "Scalar_Int_Base.hpp"
class Scalar_Int_Plain : public Scalar_Int_Base
{
    Q_OBJECT
public:
    QString caption() const override
    {
        return QStringLiteral("int Plain Number Input Buffer");
    }
};