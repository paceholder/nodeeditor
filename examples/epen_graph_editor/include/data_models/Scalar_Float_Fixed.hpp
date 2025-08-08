#pragma once
#include "OutputUIBufferBase.hpp"
class Scalar_Float_Fixed : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(float Value MEMBER _value NOTIFY propertyChanged)
    QString caption() const override { return QStringLiteral("float Fixed Buffer"); }

    QString getVariableType(UIBufferBase::LanguageTypes language,
                            QString variableName,
                            bool isInput) override
    {
        return "float " + variableName;
    }

private:
    float _value;
};