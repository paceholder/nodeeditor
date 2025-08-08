#pragma once
#include "OutputUIBufferBase.hpp"
class Scalar_Double_Base : public OutputUIBufferBase
{
    Q_OBJECT
public:
    Q_PROPERTY(double Default_Value MEMBER _defaultValue NOTIFY propertyChanged)
    Q_PROPERTY(double Min MEMBER _min NOTIFY propertyChanged)
    Q_PROPERTY(double Max MEMBER _max NOTIFY propertyChanged)

private:
    double _defaultValue;
    double _min;
    double _max;
};