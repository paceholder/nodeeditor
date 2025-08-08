#pragma once
#include "InputUIBufferBase.hpp"
class Scalar_Boolean_Fixed : public InputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(bool Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("boolean Checkbox Fixed Buffer"); }

    QString getVariableType(UIBufferBase::LanguageTypes language,
                            QString variableName,
                            bool isInput) override
    {
        if (language == UIBufferBase::CUDA)
            return "uchar " + variableName;
        return "bool " + variableName;
    }

private:
    bool _value;
};