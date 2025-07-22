#pragma once
#include "OperationDataModel.hpp"

class Process : public OperationDataModel
{
    Q_OBJECT
public:
    virtual ~Process() {}

    Q_PROPERTY(QString Grid_Size_X MEMBER _gridX NOTIFY propertyChanged)
    Q_PROPERTY(QString Grid_Size_Y MEMBER _gridY NOTIFY propertyChanged)
    Q_PROPERTY(QString Grid_Size_Z MEMBER _gridZ NOTIFY propertyChanged)

    Q_PROPERTY(QString Block_Size_X MEMBER _blockX NOTIFY propertyChanged)
    Q_PROPERTY(QString Block_Size_Y MEMBER _blockY NOTIFY propertyChanged)
    Q_PROPERTY(QString Block_Size_Z MEMBER _blockZ NOTIFY propertyChanged)

    QString caption() const override { return QStringLiteral("Process"); }

    bool portCaptionVisible(PortType portType, PortIndex portIndex) const override
    {
        Q_UNUSED(portType);
        Q_UNUSED(portIndex);
        return true;
    }

    QString portCaption(PortType portType, PortIndex portIndex) const override
    {
        switch (portType) {
        case PortType::Out:

            return nodeTypeMap.at(portIndex) ? QStringLiteral("Image") : QStringLiteral("Buffer");

        default:
            break;
        }
        return QString();
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

    void setPortType(PortIndex portIndex, bool isImage)
    {
        nodeTypeMap.insert(portIndex - 1, isImage);
    }

    void removePortType(PortIndex portIndex)
    {
        if (portIndex < nodeTypeMap.size()) {
            nodeTypeMap.removeAt(portIndex);
        }
    }

private:
    QVector<bool> nodeTypeMap{true};

    QString _gridX;
    QString _gridY;
    QString _gridZ;

    QString _blockX;
    QString _blockY;
    QString _blockZ;
};