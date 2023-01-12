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
    void init() override;

    static QString Name() { return QString("TextDisplayDataModel"); }

    QString name() const override { return TextDisplayDataModel::Name(); }

public:
    void setInData(std::shared_ptr<NodeData> nodeData, PortIndex const portIndex) override;

    QWidget *embeddedWidget() override;

private:
    QLabel *_label = nullptr;

    QString _inputText;
};
