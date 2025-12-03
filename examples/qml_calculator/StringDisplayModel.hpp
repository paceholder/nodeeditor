#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtCore/QObject>
#include "StringData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class StringDisplayModel : public NodeDelegateModel
{
    Q_OBJECT
    Q_PROPERTY(QString displayedText READ displayedText NOTIFY displayedTextChanged)

public:
    QString caption() const override { return QStringLiteral("Text Display"); }
    QString name() const override { return QStringLiteral("StringDisplay"); }

    unsigned int nPorts(PortType portType) const override
    {
        return (portType == PortType::In) ? 1 : 0;
    }

    NodeDataType dataType(PortType, PortIndex) const override
    {
        return StringData{}.type();
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex) override
    {
        auto stringData = std::dynamic_pointer_cast<StringData>(data);
        if (stringData) {
            _displayedText = stringData->text();
        } else {
            _displayedText = "";
        }
        Q_EMIT displayedTextChanged();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override { return nullptr; }

    QWidget *embeddedWidget() override { return nullptr; }

    QString displayedText() const { return _displayedText; }

Q_SIGNALS:
    void displayedTextChanged();

private:
    QString _displayedText;
};
