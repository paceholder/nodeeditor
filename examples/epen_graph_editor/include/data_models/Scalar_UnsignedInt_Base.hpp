#pragma once
#include "InputUIBufferBase.hpp"
class Scalar_UnsignedInt_Base : public InputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(uint Default_Value MEMBER _defaultValue NOTIFY propertyChanged)
    Q_PROPERTY(uint Min MEMBER _min NOTIFY propertyChanged)
    Q_PROPERTY(uint Max MEMBER _max NOTIFY propertyChanged)

    QString getVariableType(UIBufferBase::LanguageTypes language,
                            QString variableName,
                            bool isInput) override
    {
        return "unsigned int " + variableName;
    }

private:
    uint _defaultValue;
    uint _min;
    uint _max;
};