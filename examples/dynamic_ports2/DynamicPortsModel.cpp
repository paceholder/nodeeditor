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
    createPort(PortType::In, std::make_shared<ANodeData>(), "A");
    createPort(PortType::In, std::make_shared<BNodeData>(), "B");
    createPort(PortType::Out, std::make_shared<ANodeData>(), "A", QtNodes::ConnectionPolicy::Many);
    createPort(PortType::Out, std::make_shared<BNodeData>(), "B", QtNodes::ConnectionPolicy::Many);
}

QJsonObject DynamicPortsModel::save() const
{
    QJsonObject modelJson = NodeDelegateModel::save();

    QJsonArray inPortsArray;
    for (unsigned int i = 0; i < nPorts(PortType::In); i++) {
        auto p = port(PortType::In, i);
        QJsonObject portJson;
        portJson["dataType"] = p.data->type();
        portJson["name"] = p.name;
        portJson["policy"] = (int) p.connectionPolicy;

        inPortsArray.append(portJson);
    }
    modelJson["inputPorts"] = inPortsArray;

    QJsonArray outPortsArray;
    for (unsigned int i = 0; i < nPorts(PortType::Out); i++) {
        auto p = port(PortType::Out, i);
        QJsonObject portJson;
        portJson["dataType"] = p.data->type();
        portJson["name"] = p.name;
        portJson["policy"] = (int) p.connectionPolicy;

        outPortsArray.append(portJson);
    }
    modelJson["outputPorts"] = outPortsArray;

    return modelJson;
}

void DynamicPortsModel::load(QJsonObject const &p)
{
    QJsonArray inputPortsArray = p["inputPorts"].toArray();
    QJsonArray outputPortsArray = p["outputPorts"].toArray();

    clearPort(PortType::In);
    clearPort(PortType::Out);

    for (auto port : inputPortsArray) {
        createPort(PortType::In,
                   (port.toObject().value("dataType").toString() == (QString) ANodeData().type())
                       ? std::shared_ptr<NodeData>(std::make_shared<ANodeData>())
                       : std::shared_ptr<NodeData>(
                           std::make_shared<BNodeData>()), // TODO: How to get the data type by id?
                   port.toObject().value("name").toString(),
                   (QtNodes::ConnectionPolicy) port.toObject().value("policy").toInt());
    }

    for (auto port : outputPortsArray) {
        createPort(PortType::Out,
                   (port.toObject().value("dataType").toString() == ANodeData().type())
                       ? std::shared_ptr<NodeData>(std::make_shared<ANodeData>())
                       : std::shared_ptr<NodeData>(std::make_shared<BNodeData>()),
                   port.toObject().value("name").toString(),
                   (QtNodes::ConnectionPolicy) port.toObject().value("policy").toInt());
    }
}

void DynamicPortsModel::setInData(std::shared_ptr<NodeData>, PortIndex const) {}

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
        _dataType->addItem(ANodeData().type());
        _dataType->addItem(BNodeData().type());
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
            auto nodeData = (_dataType->currentText() == ANodeData().type())
                                ? std::shared_ptr<NodeData>(std::make_shared<ANodeData>())
                                : std::shared_ptr<NodeData>(std::make_shared<BNodeData>());

            insertPort(_type->currentData().value<PortType>(),
                       _index->value(),
                       nodeData,
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