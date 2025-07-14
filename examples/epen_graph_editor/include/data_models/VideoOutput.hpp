#pragma once
#include "OperationDataModel.hpp"

class VideoOutput : public OperationDataModel
{
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

    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        switch (portType) {
        case PortType::In:
            if (portIndex == 0)
                return QStringLiteral("Dividend");
            else if (portIndex == 1)
                return QStringLiteral("Divisor");

            break;

        case PortType::Out:
            return QStringLiteral("Result");

        default:
            break;
        }
        return QString();
    }

    QString name() const override { return QStringLiteral("VideoOutput"); }
};