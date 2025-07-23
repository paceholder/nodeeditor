#pragma once
#include "OperationDataModel.hpp"
#include <QMetaType>
#include <QObject>

class Size : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString X MEMBER _x)
    Q_PROPERTY(QString Y MEMBER _y)
    Q_PROPERTY(QString Z MEMBER _z)

public:
    Size(QString x, QString y, QString z, QObject *parent = nullptr)
        : QObject(parent)
        , _x(x)
        , _y(y)
        , _z(z)
    {}

    // Copy constructor
    Size(const Size &other, QObject *parent = nullptr)
        : QObject(parent)
        , _x(other._x)
        , _y(other._y)
        , _z(other._z)
    {}

    // Assignment operator
    Size &operator=(const Size &other)
    {
        if (this != &other) {
            _x = other._x;
            _y = other._y;
            _z = other._z;
        }
        return *this;
    }

    // Comparison operators
    bool operator==(const Size &other) const
    {
        return _x == other._x && _y == other._y && _z == other._z;
    }

    bool operator!=(const Size &other) const { return !(*this == other); }

private:
    QString _x;
    QString _y;
    QString _z;
};

Q_DECLARE_METATYPE(Size *)

class Process : public OperationDataModel
{
    Q_OBJECT
public:
    Process()
        : _grid(new Size("", "", "", this))
        , _block(new Size("", "", "", this))
    {}
    virtual ~Process() {}

    Q_PROPERTY(Size *Grid_Size MEMBER _grid NOTIFY propertyChanged)
    Q_PROPERTY(Size *Block_Size MEMBER _block NOTIFY propertyChanged)

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

            return nodeTypeMapRight.at(portIndex) ? QStringLiteral("Image")
                                                  : QStringLiteral("Buffer");

        default:
        case PortType::In:
            return nodeTypeMapLeft.at(portIndex) ? QStringLiteral("Image")
                                                 : QStringLiteral("Buffer");
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

    void setPortTypeRight(PortIndex portIndex, bool isImage)
    {
        nodeTypeMapRight.insert(portIndex - 1, isImage);
    }

    void removePortTypeRight(PortIndex portIndex)
    {
        if (portIndex < nodeTypeMapRight.size()) {
            nodeTypeMapRight.removeAt(portIndex);
        }
    }

    void setPortTypeLeft(PortIndex portIndex, bool isImage)
    {
        nodeTypeMapLeft.insert(portIndex - 1, isImage);
    }

    void removePortTypeLeft(PortIndex portIndex)
    {
        if (portIndex < nodeTypeMapLeft.size()) {
            nodeTypeMapLeft.removeAt(portIndex);
        }
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        NodeDataType result = BUFFER_DATA_TYPE;

        switch (portType) {
        case PortType::Out:

            return nodeTypeMapRight.at(portIndex) ? IMAGE_DATA_TYPE : BUFFER_DATA_TYPE;

        default:
        case PortType::In:
            return nodeTypeMapLeft.at(portIndex) ? IMAGE_DATA_TYPE : BUFFER_DATA_TYPE;
        }
        return result;
    }

private:
    QVector<bool> nodeTypeMapRight{true};
    QVector<bool> nodeTypeMapLeft{true};

    Size *_grid;
    Size *_block;
};