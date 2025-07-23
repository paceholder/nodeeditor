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
    Q_PROPERTY(QString Name MEMBER _name NOTIFY propertyChanged)

    OperationDataModel();
    ~OperationDataModel() = default;

    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;

    QWidget *embeddedWidget() override { return nullptr; }

    virtual void deselected();

    void setNodeName(QString);

    QString name() const override { return metaObject()->className(); }

protected:
    QString _name;
signals:
    void propertyChanged();
};
