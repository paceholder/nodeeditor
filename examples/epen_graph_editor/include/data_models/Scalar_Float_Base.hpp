#pragma once
#include "UIBufferBase.hpp"
class Scalar_Float_Base : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(float Default_Value MEMBER _defaultValue NOTIFY propertyChanged)
    Q_PROPERTY(float Min MEMBER _min NOTIFY propertyChanged)
    Q_PROPERTY(float Max MEMBER _max NOTIFY propertyChanged)

private:
    float _defaultValue;
    float _min;
    float _max;
};