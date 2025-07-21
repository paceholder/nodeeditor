#pragma once
#include "OperationDataModel.hpp"
class NumberInputBufferBase : public OperationDataModel
{
    Q_OBJECT
public:
    enum class VarTypes { Float, Int, Float4 };

    Q_ENUM(VarTypes)

    virtual ~NumberInputBufferBase() {}
private:
    QtVariantProperty *_varTypeItem;
    QtVariantProperty *_defaultValueItem;
    QtVariantProperty *_minValueItem;
    QtVariantProperty *_maxValueItem;
    int _currentVarType = 0;
    float _defaultValue = 0;
    float _minValue = 0;
    float _maxValue = 0;
};