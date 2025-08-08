#pragma once
#include "Color.hpp"
#include "OutputUIBufferBase.hpp"

class Scalar_Float4_Fixed : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Scalar_Float4_Fixed()
        : _valueF(new Color(1, 1, 1, 1, this))
    {
        connect(_valueF, &Color::colorChanged, this, [this]() { setColorF(_valueF); });
    }
    Q_PROPERTY(QColor Value MEMBER _value WRITE setColor NOTIFY propertyChanged)
    Q_PROPERTY(Color *Color MEMBER _valueF WRITE setColorF NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("boolean Color Fixed Buffer"); }

    void setColor(QColor value)
    {
        _value = value;
        _valueF->setFromQColor(value);
        emit propertyChanged();
    }

    void setColorF(Color *value)
    {
        _value.setRed(value->red255());
        _value.setGreen(value->green255());
        _value.setBlue(value->blue255());
        _value.setAlpha(value->alpha255());
        emit propertyChanged();
    }

private:
    QColor _value{255, 255, 255, 255};
    Color *_valueF;
};