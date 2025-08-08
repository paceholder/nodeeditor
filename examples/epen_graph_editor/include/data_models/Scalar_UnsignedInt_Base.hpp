#pragma once
#include "OutputUIBufferBase.hpp"
class Scalar_UnsignedInt_Base : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(uint Default_Value MEMBER _defaultValue NOTIFY propertyChanged)
    Q_PROPERTY(uint Min MEMBER _min NOTIFY propertyChanged)
    Q_PROPERTY(uint Max MEMBER _max NOTIFY propertyChanged)

private:
    uint _defaultValue;
    uint _min;
    uint _max;
};