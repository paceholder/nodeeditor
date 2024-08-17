#pragma once

#include <QtCore/QObject>

#include "TextData.hpp"

#include <QtNodes/NodeDelegateModel>

#include <iostream>

using QtNodes::NodeData;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QTextEdit;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    TextModel();

public:
    QString caption() const override { return QString("Text"); }

    bool captionVisible() const override { return true; }

    static QString Name() { return QString("Text"); }

    QString name() const override { return TextModel::Name(); }

public:
    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const portIndex) override;

    void setInData(std::shared_ptr<NodeData>, PortIndex const) override;

    QWidget *embeddedWidget() override;

    bool resizable() const override { return true; }

private Q_SLOTS:

    void onTextEdited();

private:
    QTextEdit *_textEdit = nullptr;
};
