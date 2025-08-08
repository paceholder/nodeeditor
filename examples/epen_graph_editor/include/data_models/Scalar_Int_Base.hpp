#pragma once
#include "OutputUIBufferBase.hpp"
class Scalar_Int_Base : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(int Default_Value MEMBER _defaultValue NOTIFY propertyChanged)
    Q_PROPERTY(int Min MEMBER _min NOTIFY propertyChanged)
    Q_PROPERTY(int Max MEMBER _max NOTIFY propertyChanged)

    QString getVariableType(UIBufferBase::LanguageTypes language,
                            QString variableName,
                            bool isInput) override
    {
        return "int " + variableName;
    }

private:
    int _defaultValue;
    int _min;
    int _max;
};