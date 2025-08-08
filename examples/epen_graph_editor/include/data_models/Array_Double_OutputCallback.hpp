#pragma once
#include "OutputUIBufferBase.hpp"

class Array_Double_OutputCallback : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QString Value MEMBER _value NOTIFY propertyChanged)

    QString caption() const override
    {
        return QStringLiteral("Output Double Array Callback Managed Buffer");
    }

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