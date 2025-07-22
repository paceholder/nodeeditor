#pragma once
#include "OperationDataModel.hpp"
#include <QMetaType>
#include <QObject>

class Size : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString X MEMBER _x )
    Q_PROPERTY(QString Y MEMBER _y )
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
            _x=other._x;
            _y= other._y;
            _z= other._z;
        }
        return *this;
    }

    // Comparison operators
    bool operator==(const Size &other) const
    {
        return _x== other._x && _y== other._y
               && _z== other._z;
    }

    bool operator!=(const Size &other) const { return !(*this == other); }

private:
    QString _x;
    QString _y;
    QString _z;
};

// Register the type for use with Qt's meta-type system
Q_DECLARE_METATYPE(Size *)

class Process : public OperationDataModel
{
    Q_OBJECT
public:
    Process()
        : _grid(new Size("", "", "", this))
    {}
    virtual ~Process() {}

    Q_PROPERTY(Size *Grid_Size MEMBER _grid NOTIFY propertyChanged)

    /*Q_PROPERTY(QString Grid_Size_X MEMBER _gridX NOTIFY propertyChanged)
    Q_PROPERTY(QString Grid_Size_Y MEMBER _gridY NOTIFY propertyChanged)
    Q_PROPERTY(QString Grid_Size_Z MEMBER _gridZ NOTIFY propertyChanged)

    Q_PROPERTY(QString Block_Size_X MEMBER _blockX NOTIFY propertyChanged)
    Q_PROPERTY(QString Block_Size_Y MEMBER _blockY NOTIFY propertyChanged)
    Q_PROPERTY(QString Block_Size_Z MEMBER _blockZ NOTIFY propertyChanged)*/

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
    Size *_grid;
};