#pragma once

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>

#include <iostream>
#include <vector>

using QtNodes::ConnectionPolicy;
using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QLabel;

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
    void setInData(QVariant const nodeData, PortIndex const portIndex) override;

    QWidget *embeddedWidget() override;

private:
    QLabel *_label = nullptr;

    QString _inputText;
};
