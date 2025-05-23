#pragma once

#include "TextData.hpp"

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <iostream>
#include <vector>

using QtNodes::ConnectionPolicy;
using QtNodes::NodeData;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextDisplayDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    TextDisplayDataModel();

    TextDisplayDataModel(const QString &name,
                        const QString &caption = "default caption",
                        const QString &category = "default category",
                        unsigned int inCount = 1,
                         unsigned int outCount = 1);

    virtual ~TextDisplayDataModel() {}

public:

    bool captionVisible() const override { return false; }

    static QString Name() { return QString("TextDisplayDataModel"); }


public:
    unsigned int nPorts(PortType portType) const;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const port) override;

    void setInData(std::shared_ptr<NodeData> data, PortIndex const portIndex) override;

    QWidget *embeddedWidget() override { return _label; }

private:
    QLabel *_label;
    QString _inputText;
};
