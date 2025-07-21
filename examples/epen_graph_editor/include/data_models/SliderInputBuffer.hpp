#pragma once
#include "NumberInputBufferBase.hpp"
class SliderInputBuffer : public NumberInputBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(double Value1 MEMBER m_value1 NOTIFY propertyChanged)
    Q_PROPERTY(double Value2 MEMBER m_value2 NOTIFY propertyChanged)

    enum class VarTypes { Float, Int, Float4 };
    SliderInputBuffer() { displayNameMap["Value1"] = "My custom value"; }
    Q_ENUM(VarTypes)

    virtual ~SliderInputBuffer() {}

    QString caption() const override { return QStringLiteral("Slider Input Buffer"); }

    bool portCaptionVisible(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType);
        Q_UNUSED(portIndex);
        return true;
    }

    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        switch (portType) {
        case PortType::Out:
            return QStringLiteral("Video");

        default:
            break;
        }
        return QString();
    }

    QString name() const override { return QStringLiteral("SliderBuffer"); }

    unsigned int nPorts(PortType portType) const override
    {
        unsigned int result;

        if (portType == PortType::In)
            result = 0;
        else
            result = 1;

        return result;
    }

private:
    double m_value1 = 0.123;
    double m_value2 = 42;
};