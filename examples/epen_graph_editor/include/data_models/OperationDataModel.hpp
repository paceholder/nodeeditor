#pragma once

#include <QtNodes/NodeDelegateModel>

#include <iostream>
#include <QMetaEnum>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

static const NodeDataType IMAGE_DATA_TYPE{"image", "Image"};
static const NodeDataType BUFFER_DATA_TYPE{"buffer", "BUFFER"};

class OperationDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    Q_PROPERTY(QString Name READ getName WRITE setName NOTIFY propertyChanged)

    OperationDataModel();
    ~OperationDataModel() = default;

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;

    QWidget *embeddedWidget() override { return nullptr; }

    void setNodeName(QString);

    QString name() const override { return metaObject()->className(); }
    QString getName() { return _name; }

protected:
    QString _name;
    virtual void setName(QString newName) { _name = newName; }
signals:
    void propertyChanged();
};
