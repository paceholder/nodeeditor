#pragma once
#include "UIBufferBase.hpp"
class Scalar_UnsignedInt_Base : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(int Default_Value MEMBER _defaultValue WRITE setDefaultValue NOTIFY propertyChanged)
    Q_PROPERTY(int Min MEMBER _min WRITE setMinValue NOTIFY propertyChanged)
    Q_PROPERTY(int Max MEMBER _min WRITE setMaxValue NOTIFY propertyChanged)

    void setDefaultValue(int v)
    {
        if (v < 0)
            v = 0;
        _defaultValue = v;
    }

    void setMinValue(int v)
    {
        if (v < 0)
            v = 0;
        _min = v;
    }

    void setMaxValue(int v)
    {
        if (v < 0)
            v = 0;
        _max = v;
    }

private:
    uint _defaultValue;
    uint _min;
    uint _max;
};