#pragma once
#include "OperationDataModel.hpp"
class UIBufferBase : public OperationDataModel
{
    Q_OBJECT
public:
    virtual ~UIBufferBase() {}

    enum LanguageTypes {
        CUDA,
        OPENCL,
        METAL,
    };

    virtual QString getVariableType(UIBufferBase::LanguageTypes language,
                                    QString variableName,
                                    bool isInput)
    {
        return " UNDEFINED " + variableName;
    }
};