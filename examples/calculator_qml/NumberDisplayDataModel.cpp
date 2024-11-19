#include "NumberDisplayDataModel.hpp"
#include <QQuickItem>
#include <QQuickWidget>
#include <QUrl>
#include <QVariant>
#include <QtNodes/NodeDelegateModel>

unsigned int NumberDisplayDataModel::nPorts(PortType portType) const
{
    unsigned int result = 1;

    switch (portType) {
    case PortType::In:
        result = 1;
        break;

    case PortType::Out:
        result = 0;

    default:
        break;
    }

    return result;
}

NodeDataType NumberDisplayDataModel::dataType(PortType, PortIndex) const
{
    return DecimalData().type();
}

std::shared_ptr<NodeData> NumberDisplayDataModel::outData(PortIndex)
{
    std::shared_ptr<NodeData> ptr;
    return ptr;
}

void NumberDisplayDataModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
    _numberData = std::dynamic_pointer_cast<DecimalData>(data);

    if (!_label)
        return;

    if (_numberData) {
        _label->setProperty("text", _numberData->numberAsText());
    } else {
        _label->setProperty("text", "");
    }
}

QWidget *NumberDisplayDataModel::embeddedWidget()
{
    if (!_label) {
        _qwid = new QQuickWidget(nullptr);
        _qwid->setSource(QUrl("qrc:/hello/Display.qml"));
        _qwid->setFixedWidth(70);
        _qwid->setFixedHeight(20);
        _label = _qwid->rootObject()->findChild<QObject *>("label");
    }

    return _qwid;
}

double NumberDisplayDataModel::number() const
{
    if (_numberData)
        return _numberData->number();

    return 0.0;
}
