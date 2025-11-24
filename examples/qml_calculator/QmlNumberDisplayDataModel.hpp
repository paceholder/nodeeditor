#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtCore/QObject>
#include "DecimalData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QmlNumberDisplayDataModel : public NodeDelegateModel
{
    Q_OBJECT
    Q_PROPERTY(QString displayedText READ displayedText NOTIFY displayedTextChanged)

public:
    QmlNumberDisplayDataModel() {}

    QString caption() const override { return QStringLiteral("Result"); }
    QString name() const override { return QStringLiteral("NumberDisplay"); }
    bool captionVisible() const override { return false; }

    unsigned int nPorts(PortType portType) const override {
        return (portType == PortType::In) ? 1 : 0;
    }

    NodeDataType dataType(PortType, PortIndex) const override {
        return DecimalData().type();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override { return nullptr; }

    void setInData(std::shared_ptr<NodeData> data, PortIndex) override {
        auto numberData = std::dynamic_pointer_cast<DecimalData>(data);
        if (numberData) {
            _displayedText = numberData->numberAsText();
        } else {
            _displayedText = "---";
        }
        Q_EMIT displayedTextChanged();
    }

    QWidget *embeddedWidget() override { return nullptr; }

    QString displayedText() const { return _displayedText; }

Q_SIGNALS:
    void displayedTextChanged();

private:
    QString _displayedText = "---";
};
