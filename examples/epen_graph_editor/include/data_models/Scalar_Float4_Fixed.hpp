#pragma once
#include "UIBufferBase.hpp"
class Scalar_Float4_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QColor Value MEMBER _value WRITE setColor NOTIFY propertyChanged)
    Q_PROPERTY(double Red MEMBER _red WRITE setRed NOTIFY propertyChanged)
    Q_PROPERTY(double Green MEMBER _green WRITE setGreen NOTIFY propertyChanged)
    Q_PROPERTY(double Blue MEMBER _blue WRITE setBlue NOTIFY propertyChanged)
    Q_PROPERTY(double Alpha MEMBER _alpha WRITE setAlpha NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("boolean Color Fixed Buffer"); }

    void setRed(double v)
    {
        _red = (float) v;
        _value.setRedF(_red);
    }

    void setGreen(double v)
    {
        _green = (float) v;
        _value.setGreenF(_green);
    }

    void setBlue(double v)
    {
        _blue = (float) v;
        _value.setBlueF(_blue);
    }

    void setAlpha(double v)
    {
        _alpha = (float) v;
        _value.setAlphaF(_alpha);
    }

    void setColor(QColor value)
    {
        _red = value.redF();
        _green = value.greenF();
        _blue = value.blueF();
        _alpha = value.alphaF();
        _value = value;
    }

private:
    QColor _value{255, 255, 255, 255};
    float _red = 1.0f;
    float _green = 1.0f;
    float _blue = 1.0f;
    float _alpha = 1.0f;
};