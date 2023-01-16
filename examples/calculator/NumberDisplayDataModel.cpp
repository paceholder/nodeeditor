#include "NumberDisplayDataModel.hpp"

#include <QtWidgets/QLabel>

NumberDisplayDataModel::NumberDisplayDataModel()
    : _label{nullptr}
{}

void NumberDisplayDataModel::init()
{
    createPort(PortType::In, "decimal", "Decimal");
}

void NumberDisplayDataModel::setInData(QVariant const nodeData, PortIndex portIndex)
{
    if (!nodeData.isNull())
        _number = nodeData.toDouble();
    else
        _number = 0.0;

    if (!_label)
        return;

    _label->setText(QString::number(_number, 'f'));

    _label->adjustSize();
}

QWidget *NumberDisplayDataModel::embeddedWidget()
{
    if (!_label) {
        _label = new QLabel();
        _label->setMargin(3);
    }
    return _label;
}

double NumberDisplayDataModel::number() const
{
    return _number;
}
