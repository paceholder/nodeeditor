#pragma once

#include "ClutterFilterWidget.hpp"
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
class ClutterFilterModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    ~ClutterFilterModel() override = default;

public:
    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex port) override;

    void setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) override;

    QWidget *embeddedWidget() override;

    QString caption() const override { return QStringLiteral("ClutterFilter"); }

    QString name() const override { return QStringLiteral("ClutterFilter"); }

    QJsonObject save() const override;

    void load(QJsonObject const &) override;

private:
    ClutterFilterWidget *_widget;
    float _value;
};
