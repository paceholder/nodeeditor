#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtCore/QObject>
#include "BooleanData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class BooleanDisplayModel : public NodeDelegateModel
{
    Q_OBJECT
    Q_PROPERTY(QString displayedText READ displayedText NOTIFY displayedTextChanged)
    Q_PROPERTY(bool value READ value NOTIFY displayedTextChanged)

public:
    QString caption() const override { return QStringLiteral("Bool Display"); }
    QString name() const override { return QStringLiteral("BooleanDisplay"); }

    unsigned int nPorts(PortType portType) const override
    {
        return (portType == PortType::In) ? 1 : 0;
    }

    NodeDataType dataType(PortType, PortIndex) const override
    {
        return BooleanData{}.type();
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex) override
    {
        auto boolData = std::dynamic_pointer_cast<BooleanData>(data);
        if (boolData) {
            _value = boolData->value();
            _displayedText = _value ? "TRUE" : "FALSE";
        } else {
            _value = false;
            _displayedText = "...";
        }
        Q_EMIT displayedTextChanged();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override { return nullptr; }

    QWidget *embeddedWidget() override { return nullptr; }

    QString displayedText() const { return _displayedText; }
    bool value() const { return _value; }

Q_SIGNALS:
    void displayedTextChanged();

private:
    QString _displayedText = "...";
    bool _value = false;
};
