#pragma once
#include "OperationDataModel.hpp"

class CallbackManagedImageBase : public OperationDataModel
{
    Q_OBJECT
public:
    Q_PROPERTY(float Width MEMBER _width NOTIFY propertyChanged)
    Q_PROPERTY(float Height MEMBER _height NOTIFY propertyChanged)

    virtual ~CallbackManagedImageBase() {}

    bool portCaptionVisible(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType);
        Q_UNUSED(portIndex);
        return true;
    }

    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        return QStringLiteral("Image");
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        return IMAGE_DATA_TYPE;
    }

private:
    float _width;
    float _height;
};