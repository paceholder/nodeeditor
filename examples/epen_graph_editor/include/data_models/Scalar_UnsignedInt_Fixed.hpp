#pragma once
#include "InputUIBufferBase.hpp"
class Scalar_UnsignedInt_Fixed : public InputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(uint Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("unsigned int Fixed Buffer"); }

    QString getVariableType(UIBufferBase::LanguageTypes language,
                            QString variableName,
                            bool isInput) override
    {
        return "unsigned int " + variableName;
    }

private:
    uint _value;
};