#pragma once
#include "UIBufferBase.hpp"

class Scalar_Float4_Color : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(QColor Default_Value MEMBER _defaultValue WRITE setColor NOTIFY propertyChanged)
    Q_PROPERTY(float Default_Red MEMBER _red WRITE setRed NOTIFY propertyChanged)
    Q_PROPERTY(float Default_Green MEMBER _green WRITE setGreen NOTIFY propertyChanged)
    Q_PROPERTY(float Default_Blue MEMBER _blue WRITE setBlue NOTIFY propertyChanged)
    Q_PROPERTY(float Default_Alpha MEMBER _alpha WRITE setAlpha NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("boolean Color Input Buffer"); }

    void setRed(float v)
    {
        if (v > 1)
            v = 1;
        if (v < 0)
            v = 0;
        _red = (float) v;
        _defaultValue.setRedF(_red);
    }

    void setGreen(float v)
    {
        if (v > 1)
            v = 1;
        if (v < 0)
            v = 0;
        _green = (float) v;
        _defaultValue.setGreenF(_green);
    }

    void setBlue(float v)
    {
        if (v > 1)
            v = 1;
        if (v < 0)
            v = 0;
        _blue = (float) v;
        _defaultValue.setBlueF(_blue);
    }

    void setAlpha(float v)
    {
        if (v > 1)
            v = 1;
        if (v < 0)
            v = 0;
        _alpha = (float) v;
        _defaultValue.setAlphaF(_alpha);
    }

    void setColor(QColor value)
    {
        _red = value.redF();
        _green = value.greenF();
        _blue = value.blueF();
        _alpha = value.alphaF();
        _defaultValue = value;
    }

private:
    QColor _defaultValue{255, 255, 255, 255};
    float _red = 1.0f;
    float _green = 1.0f;
    float _blue = 1.0f;
    float _alpha = 1.0f;
};