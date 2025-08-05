#include "data_models/Process.hpp"

Process::Process()
    : _grid(new Size("", "", "", this))
    , _block(new Size("", "", "", this))
{}

Process::~Process() {}

QString Process::caption() const
{
    return QStringLiteral("Process");
}

bool Process::portCaptionVisible(PortType portType, PortIndex portIndex) const
{
    Q_UNUSED(portType);
    Q_UNUSED(portIndex);
    return true;
}

QString Process::portCaption(PortType portType, PortIndex portIndex) const
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

unsigned int Process::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 0;
    else
        result = 0;

    return result;
}

void Process::setPortTypeRight(PortIndex portIndex, bool isImage)
{
    if (isImage) {
        _rightPorts.insert(portIndex - 1, new ImagePort(100, 100, this));
    } else {
        _rightPorts.insert(portIndex - 1, new BufferPort(1024, this));
    }
}

void Process::removePortTypeRight(PortIndex portIndex)
{
    if (portIndex < _rightPorts.size()) {
        _rightPorts.at(portIndex);
        _rightPorts.removeAt(portIndex);
    }
}

void Process::setPortTypeLeft(PortIndex portIndex, bool isImage)
{
    if (isImage) {
        _leftPorts.insert(portIndex - 1, new ImagePort(100, 100, this));
    } else {
        _leftPorts.insert(portIndex - 1, new BufferPort(1024, this));
    }
}

void Process::removePortTypeLeft(PortIndex portIndex)
{
    if (portIndex < _leftPorts.size()) {
        delete _leftPorts.at(portIndex);
        _leftPorts.removeAt(portIndex);
    }
}

NodeDataType Process::dataType(PortType portType, PortIndex portIndex) const
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

QObject *Process::findPort(int portIndex, bool isRightPort)
{
    if (isRightPort) {
        return _rightPorts.at(portIndex);
    }
    return _leftPorts.at(portIndex);
}