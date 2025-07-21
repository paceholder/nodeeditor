#pragma once
#include "UIBufferBase.hpp"
class Scalar_Int_Base : public UIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(int Default_Value MEMBER _defaultValue NOTIFY propertyChanged)
    Q_PROPERTY(int Min MEMBER _min NOTIFY propertyChanged)
    Q_PROPERTY(int Max MEMBER _max NOTIFY propertyChanged)

private:
    uint _defaultValue;
    uint _min;
    uint _max;
};