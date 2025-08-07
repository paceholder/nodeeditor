#ifndef BUFFERPORT_HPP
#define BUFFERPORT_HPP

#include "PortBase.hpp"

class BufferPort : public PortBase
{
    Q_OBJECT
public:
    Q_PROPERTY(uint Length MEMBER _length NOTIFY propertyChanged)

    bool isImage() override { return false; }

public:
    BufferPort(QString name, uint length, QObject *parent = nullptr)
        : PortBase(name, parent)
        , _length(length)
    {}

    // Copy constructor
    BufferPort(const BufferPort &other, QObject *parent = nullptr)
        : PortBase(other._name, parent)
        , _length(other._length)
    {}

    // Assignment operator
    BufferPort &operator=(const BufferPort &other)
    {
        if (this != &other) {
            _length = other._length;
            _name = other._name;
        }
        return *this;
    }

    // Comparison operators
    bool operator==(const BufferPort &other) const
    {
        return _length == other._length && _name == other._name;
    }

    bool operator!=(const BufferPort &other) const { return !(*this == other); }

private:
    uint _length;
};

#endif