#ifndef IMAGEPORT_HPP
#define IMAGEPORT_HPP
#include "PortBase.hpp"

class ImagePort : public PortBase
{
    Q_OBJECT
public:
    Q_PROPERTY(float Width MEMBER _width NOTIFY propertyChanged)
    Q_PROPERTY(float Height MEMBER _height NOTIFY propertyChanged)

    ImagePort(QString name, float width, float height, QObject *parent = nullptr)
        : PortBase(name, parent)
        , _width(width)
        , _height(height)
    {}

    // Copy constructor
    ImagePort(const ImagePort &other, QObject *parent = nullptr)
        : PortBase(other._name, parent)
        , _width(other._width)
        , _height(other._height)
    {}

    // Assignment operator
    ImagePort &operator=(const ImagePort &other)
    {
        if (this != &other) {
            _width = other._width;
            _height = other._height;
            _name = other._name;
        }
        return *this;
    }

    // Comparison operators
    bool operator==(const ImagePort &other) const
    {
        return _width == other._width && _height == other._height && _name == other._name;
    }

    bool operator!=(const ImagePort &other) const { return !(*this == other); }

private:
    float _width;
    float _height;
};
#endif