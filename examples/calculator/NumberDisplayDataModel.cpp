#include "NumberDisplayDataModel.hpp"

#include <QtWidgets/QLabel>

NumberDisplayDataModel::NumberDisplayDataModel()
    : _label{nullptr}
{}

void NumberDisplayDataModel::init()
{
    createPort(PortType::In, std::make_shared<DecimalData>(), "Decimal");
}

void NumberDisplayDataModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
    _numberData = std::dynamic_pointer_cast<DecimalData>(data);

    if (!_label)
        return;

    if (_numberData) {
        _label->setText(_numberData->numberAsText());
    } else {
        _label->clear();
    }

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
    if (_numberData)
        return _numberData->number();

    return 0.0;
}
