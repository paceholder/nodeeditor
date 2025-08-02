#pragma once
#include "OperationDataModel.hpp"
#include <QMetaType>
#include <QObject>

class PortBase : public QObject
{
    Q_OBJECT
public:
    PortBase(QObject *parent = nullptr)
        : QObject(parent)
    {}
    virtual bool isImage() { return true; }
signals:
    void propertyChanged();
};

class ImagePort : public PortBase
{
    Q_OBJECT
public:
    Q_PROPERTY(float Width MEMBER _width NOTIFY propertyChanged)
    Q_PROPERTY(float Height MEMBER _height NOTIFY propertyChanged)

    ImagePort(float width, float height, QObject *parent = nullptr)
        : PortBase(parent)
        , _width(width)
        , _height(height)
    {}

    // Copy constructor
    ImagePort(const ImagePort &other, QObject *parent = nullptr)
        : PortBase(parent)
        , _width(other._width)
        , _height(other._height)
    {}

    // Assignment operator
    ImagePort &operator=(const ImagePort &other)
    {
        if (this != &other) {
            _width = other._width;
            _height = other._height;
        }
        return *this;
    }

    // Comparison operators
    bool operator==(const ImagePort &other) const
    {
        return _width == other._width && _height == other._height;
    }

    bool operator!=(const ImagePort &other) const { return !(*this == other); }

private:
    float _width;
    float _height;
};

class BufferPort : public PortBase
{
    Q_OBJECT
public:
    Q_PROPERTY(uint Length MEMBER _length NOTIFY propertyChanged)

    bool isImage() override { return false; }

public:
    BufferPort(uint length, QObject *parent = nullptr)
        : PortBase(parent)
        , _length(length)
    {}

    // Copy constructor
    BufferPort(const BufferPort &other, QObject *parent = nullptr)
        : PortBase(parent)
        , _length(other._length)
    {}

    // Assignment operator
    BufferPort &operator=(const BufferPort &other)
    {
        if (this != &other) {
            _length = other._length;
        }
        return *this;
    }

    // Comparison operators
    bool operator==(const BufferPort &other) const { return _length == other._length; }

    bool operator!=(const BufferPort &other) const { return !(*this == other); }

private:
    uint _length;
};

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
Q_DECLARE_METATYPE(BufferPort *)
Q_DECLARE_METATYPE(ImagePort *)

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

            return _rightPorts.at(portIndex)->isImage() ? QStringLiteral("Image")
                                                        : QStringLiteral("Buffer");

        default:
        case PortType::In:
            return _leftPorts.at(portIndex)->isImage() ? QStringLiteral("Image")
                                                       : QStringLiteral("Buffer");
        }
        return QString();
    }

    unsigned int nPorts(PortType portType) const override
    {
        unsigned int result;

        if (portType == PortType::In)
            result = 0;
        else
            result = 0;

        return result;
    }

    void setPortTypeRight(PortIndex portIndex, bool isImage)
    {
        if (isImage) {
            _rightPorts.insert(portIndex - 1, new ImagePort(100, 100, this));
        } else {
            _rightPorts.insert(portIndex - 1, new BufferPort(1024, this));
        }
    }

    void removePortTypeRight(PortIndex portIndex)
    {
        if (portIndex < _rightPorts.size()) {
            _rightPorts.at(portIndex);
            _rightPorts.removeAt(portIndex);
        }
    }

    void setPortTypeLeft(PortIndex portIndex, bool isImage)
    {
        if (isImage) {
            _leftPorts.insert(portIndex - 1, new ImagePort(100, 100, this));
        } else {
            _leftPorts.insert(portIndex - 1, new BufferPort(1024, this));
        }
    }

    void removePortTypeLeft(PortIndex portIndex)
    {
        if (portIndex < _leftPorts.size()) {
            delete _leftPorts.at(portIndex);
            _leftPorts.removeAt(portIndex);
        }
    }

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override
    {
        NodeDataType result = BUFFER_DATA_TYPE;

        switch (portType) {
        case PortType::Out:

            return _rightPorts.at(portIndex)->isImage() ? IMAGE_DATA_TYPE : BUFFER_DATA_TYPE;

        default:
        case PortType::In:
            return _leftPorts.at(portIndex)->isImage() ? IMAGE_DATA_TYPE : BUFFER_DATA_TYPE;
        }
        return result;
    }

    QObject *findPort(int portIndex, bool isRightPort)
    {
        if (isRightPort) {
            return _rightPorts.at(portIndex);
        }
        return _leftPorts.at(portIndex);
    }

private:
    Size *_grid;
    Size *_block;

    QVector<PortBase *> _leftPorts{};
    QVector<PortBase *> _rightPorts{};
};