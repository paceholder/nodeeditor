#pragma once
#include "InputUIBufferBase.hpp"

class Array_Double_Fixed : public InputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QString Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("Double Array Fixed Buffer"); }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        return BUFFER_DATA_TYPE;
    }

    QString getVariableType(UIBufferBase::LanguageTypes language,
                            QString variableName,
                            bool isInput) override
    {
        return "double* " + variableName;
    }

private:
    QString _value;
};