#include "DynamicPortsModel.hpp"

#include <QJsonArray>

DynamicPortsModel::~DynamicPortsModel()
{
    delete _type;
    delete _index;
    delete _name;
    delete _policy;
    delete _dataType;
    delete _insertPort;
    delete _removePort;

    delete _layout;
    delete _widget;
}

void DynamicPortsModel::init()
{
    createPort(PortType::In, "AData", "A");
    createPort(PortType::In, "BData", "B");
    createPort(PortType::Out, "AData", "A", QtNodes::ConnectionPolicy::Many);
    createPort(PortType::Out, "BData", "B", QtNodes::ConnectionPolicy::Many);
}

QWidget *DynamicPortsModel::embeddedWidget()
{
    if (!_widget) {
        _widget = new QWidget();
        _layout = new QGridLayout(_widget);

        _type = new QComboBox();
        _type->addItem("IN", QVariant::fromValue(PortType::In));
        _type->addItem("OUT", QVariant::fromValue(PortType::Out));
        _type->setCurrentIndex(1);
        _layout->addWidget(_type, 0, 0);

        _index = new QSpinBox();
        _index->setValue(0);
        _layout->addWidget(_index, 0, 1);

        _dataType = new QComboBox();
        _dataType->addItem("AData");
        _dataType->addItem("BData");
        _layout->addWidget(_dataType, 1, 0, 1, 2);

        _name = new QLineEdit("data");
        _layout->addWidget(_name, 2, 0);

        _policy = new QComboBox();
        _policy->addItem("One", QVariant::fromValue(QtNodes::ConnectionPolicy::One));
        _policy->addItem("Many", QVariant::fromValue(QtNodes::ConnectionPolicy::Many));
        _policy->setCurrentIndex(1);
        _layout->addWidget(_policy, 2, 1);

        connect(_type, SIGNAL(activated(int)), _policy, SLOT(setCurrentIndex(int)));

        _insertPort = new QPushButton("+");
        connect(_insertPort, &QPushButton::clicked, this, [this]() {
            insertPort(_type->currentData().value<PortType>(),
                       _index->value(),
                       _dataType->currentText(),
                       _name->text(),
                       _policy->currentData().value<QtNodes::ConnectionPolicy>());
            _widget->adjustSize();
        });
        _layout->addWidget(_insertPort, 3, 0);

        _removePort = new QPushButton("-");
        connect(_removePort, &QPushButton::clicked, this, [this]() {
            removePort(_type->currentData().value<PortType>(), _index->value());
            _widget->adjustSize();
        });
        _layout->addWidget(_removePort, 3, 1);
    }
    return _widget;
}