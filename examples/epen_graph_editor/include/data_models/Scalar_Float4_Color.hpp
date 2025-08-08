#pragma once
#include "Color.hpp"
#include "OutputUIBufferBase.hpp"

class Scalar_Float4_Color : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Scalar_Float4_Color()
        : _defaultValueF(new Color(1, 1, 1, 1, this))
    {
        connect(_defaultValueF, &Color::colorChanged, this, [this]() { setColorF(_defaultValueF); });
    }

    Q_PROPERTY(QColor Default_Value MEMBER _defaultValue WRITE setColor NOTIFY propertyChanged)
    Q_PROPERTY(Color *Default_Color MEMBER _defaultValueF WRITE setColorF NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("boolean Color Input Buffer"); }

    void setColor(QColor value)
    {
        _defaultValue = value;
        _defaultValueF->setFromQColor(value);
        emit propertyChanged();
    }

    void setColorF(Color *value)
    {
        _defaultValue.setRed(value->red255());
        _defaultValue.setGreen(value->green255());
        _defaultValue.setBlue(value->blue255());
        _defaultValue.setAlpha(value->alpha255());
        emit propertyChanged();
    }

private:
    QColor _defaultValue{255, 255, 255, 255};
    Color *_defaultValueF;
};