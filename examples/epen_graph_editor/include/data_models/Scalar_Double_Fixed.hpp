#pragma once
#include "OutputUIBufferBase.hpp"
class Scalar_Double_Fixed : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(double Value MEMBER _value NOTIFY propertyChanged)
    QString caption() const override { return QStringLiteral("double Fixed Buffer"); }

    QString getVariableType(UIBufferBase::LanguageTypes language,
                            QString variableName,
                            bool isInput) override
    {
        return "double " + variableName;
    }

private:
    double _value;
};