#pragma once

#include <QtNodes/NodeDelegateModel>

#include "VideoData.hpp"
#include "qttreepropertybrowser.h"
#include "qteditorfactory.h"
#include <iostream>
#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>

class VideoData;

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class OperationDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    OperationDataModel();
    ~OperationDataModel() = default;

public:
    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;

    QWidget *embeddedWidget() override { return nullptr; }
    virtual void setupProperties(QtAbstractPropertyBrowser*);

    virtual void deselected();

protected:
    std::weak_ptr<VideoData> _number1;
    std::weak_ptr<VideoData> _number2;

    std::shared_ptr<VideoData> _result;
    QString _name;
};
