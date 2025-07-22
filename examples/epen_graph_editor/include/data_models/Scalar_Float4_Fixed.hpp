#pragma once
#include "UIBufferBase.hpp"
class Scalar_Float4_Fixed : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QColor Value MEMBER _value WRITE setColor NOTIFY propertyChanged)
    Q_PROPERTY(float Red MEMBER _red WRITE setRed NOTIFY propertyChanged)
    Q_PROPERTY(float Green MEMBER _green WRITE setGreen NOTIFY propertyChanged)
    Q_PROPERTY(float Blue MEMBER _blue WRITE setBlue NOTIFY propertyChanged)
    Q_PROPERTY(float Alpha MEMBER _alpha WRITE setAlpha NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("boolean Color Fixed Buffer"); }

    void setRed(float v)
    {
        if (v > 1)
            v = 1;
        if (v < 0)
            v = 0;
        _red = (float) v;
        _value.setRedF(_red);
    }

    void setGreen(float v)
    {
        if (v > 1)
            v = 1;
        if (v < 0)
            v = 0;
        _green = (float) v;
        _value.setGreenF(_green);
    }

    void setBlue(float v)
    {
        if (v > 1)
            v = 1;
        if (v < 0)
            v = 0;
        _blue = (float) v;
        _value.setBlueF(_blue);
    }

    void setAlpha(float v)
    {
        if (v > 1)
            v = 1;
        if (v < 0)
            v = 0;
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