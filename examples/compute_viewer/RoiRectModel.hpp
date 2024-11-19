#pragma once

#include "RoiRectWidget.hpp"
#include <memory>
#include <QJsonObject>
#include <QObject>
#include <QString>
#include <QStringLiteral>
#include <QWidget>
#include <QtNodes/NodeDelegateModel>

using QtNodes::ConnectionPolicy;
using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class RoiRectModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    ~RoiRectModel() override = default;

public:
    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;

    QWidget *embeddedWidget() override;

    QString caption() const override { return QStringLiteral("Roi"); }

    QString name() const override { return QStringLiteral("RoiRect"); }

    QJsonObject save() const override;

    void load(QJsonObject const &) override;

private:
    RoiRectWidget *_widget;
    float _x1;
    float _x2;
    float _y1;
    float _y2;
    float _z1;
    float _z2;
};
