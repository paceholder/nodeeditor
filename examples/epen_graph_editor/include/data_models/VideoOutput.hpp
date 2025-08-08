#pragma once
#include "OperationDataModel.hpp"

class VideoOutput : public OperationDataModel
{
    Q_OBJECT
public:
    virtual ~VideoOutput() {}

public:
    QString caption() const override { return QStringLiteral("Video Output"); }

    bool portCaptionVisible(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType);
        Q_UNUSED(portIndex);
        return true;
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        return IMAGE_DATA_TYPE;
    }

    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        return QStringLiteral("Image");
    }

    unsigned int nPorts(PortType portType) const override
    {
        unsigned int result;

        if (portType == PortType::In)
            result = 1;
        else
            result = 1;

        return result;
    }
};