#ifndef OUTPUTIMAGEPORT_HPP
#define OUTPUTIMAGEPORT_HPP
#include "ImagePort.hpp"

class OutputImagePort : public ImagePort
{
    Q_OBJECT
public:
    Q_PROPERTY(OutputImagePort::AccessType Access_Type MEMBER _accessType NOTIFY propertyChanged)

    enum AccessType { ReadWrite, Write };
    Q_ENUM(AccessType)

    OutputImagePort(QString name, float width, float height, QObject *parent = nullptr)
        : ImagePort(name, width, height, parent)
    {}

    OutputImagePort(const OutputImagePort &other, QObject *parent = nullptr)
        : ImagePort(other, parent)
    {}

    OutputImagePort &operator=(const OutputImagePort &other)
    {
        if (this != &other) {
            ImagePort::operator=(other);
            _accessType = other._accessType;
        }
        return *this;
    }

    bool operator==(const OutputImagePort &other) const
    {
        return ImagePort::operator==(other) && _accessType == other._accessType;
    }

    bool operator!=(const OutputImagePort &other) const { return !(*this == other); }

private:
    AccessType _accessType;
};
#endif