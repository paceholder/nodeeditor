#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtCore/QObject>
#include "DecimalData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QmlNumberSourceDataModel : public NodeDelegateModel
{
    Q_OBJECT
    Q_PROPERTY(double number READ number WRITE setNumber NOTIFY numberChanged)

public:
    QmlNumberSourceDataModel() : _number(std::make_shared<DecimalData>(0.0)) {}

    QString caption() const override { return QStringLiteral("Number Source"); }
    QString name() const override { return QStringLiteral("NumberSource"); }
    bool captionVisible() const override { return false; }

    unsigned int nPorts(PortType portType) const override {
        return (portType == PortType::Out) ? 1 : 0;
    }

    NodeDataType dataType(PortType, PortIndex) const override {
        return DecimalData().type();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override {
        return _number;
    }

    void setInData(std::shared_ptr<NodeData>, PortIndex) override {}

    QWidget *embeddedWidget() override { return nullptr; }

    double number() const { return _number->number(); }

    void setNumber(double n) {
        if (_number->number() != n) {
            _number = std::make_shared<DecimalData>(n);
            Q_EMIT dataUpdated(0);
            Q_EMIT numberChanged();
        }
    }

    QJsonObject save() const override {
        QJsonObject modelJson = NodeDelegateModel::save();
        modelJson["number"] = QString::number(_number->number());
        return modelJson;
    }

    void load(QJsonObject const &p) override {
        QJsonValue v = p["number"];
        if (!v.isUndefined()) {
            QString strNum = v.toString();
            bool ok;
            double d = strNum.toDouble(&ok);
            if (ok) {
                setNumber(d);
            }
        }
    }

Q_SIGNALS:
    void numberChanged();

private:
    std::shared_ptr<DecimalData> _number;
};
