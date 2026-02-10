#pragma once

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include "TextData.hpp"

using QtNodes::NodeData;
using QtNodes::NodeDataType;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// A simple text display node
class TextDisplayModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    TextDisplayModel()
        : _label(nullptr)
    {}

    QString caption() const override { return QStringLiteral("Text Display"); }

    QString name() const override { return QStringLiteral("TextDisplay"); }

    unsigned int nPorts(PortType portType) const override
    {
        if (portType == PortType::In)
            return 1;
        return 0;
    }

    NodeDataType dataType(PortType, PortIndex) const override
    {
        return TextData{}.type();
    }

    std::shared_ptr<NodeData> outData(PortIndex) override
    {
        return nullptr;
    }

    void setInData(std::shared_ptr<NodeData> data, PortIndex) override
    {
        auto textData = std::dynamic_pointer_cast<TextData>(data);
        if (textData && !textData->isEmpty()) {
            _label->setText(textData->text());
        } else {
            _label->setText("No data");
        }
    }

    QWidget *embeddedWidget() override
    {
        if (!_label) {
            _label = new QLabel("No data");
            _label->setMinimumWidth(100);
            _label->setAlignment(Qt::AlignCenter);
            _label->setStyleSheet("QLabel { background-color: #333; padding: 5px; }");
        }
        return _label;
    }

private:
    QLabel *_label;
};
