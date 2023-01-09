#pragma once

#include <QtCore/QObject>

#include "TextData.hpp"

#include <QtNodes/NodeDelegateModel>

#include <iostream>

using QtNodes::NodeData;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QLineEdit;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class TextSourceDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    TextSourceDataModel();

public:
    QString caption() const override { return QString("Text Source"); }

    bool captionVisible() const override { return false; }

    static QString Name() { return QString("TextSourceDataModel"); }

    QString name() const override { return TextSourceDataModel::Name(); }

public:
    unsigned int nPorts(PortType portType) const override;

    NodeDataType dataType(PortType portType, PortIndex portIndex) const override;

    std::shared_ptr<NodeData> outData(PortIndex const portIndex) override;

    void setInData(std::shared_ptr<NodeData>, PortIndex const) override {}

    QWidget *embeddedWidget() override;

private Q_SLOTS:

    void onTextEdited(QString const &string);

private:
    QLineEdit *_lineEdit;
};
