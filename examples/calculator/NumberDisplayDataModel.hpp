#pragma once

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>

#include <iostream>

using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

class QLabel;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class NumberDisplayDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    NumberDisplayDataModel();

    ~NumberDisplayDataModel() = default;

public:
    void init() override;

    QString name() const override { return QStringLiteral("Result"); }

public:
    void setInData(QVariant const nodeData, PortIndex portIndex) override;

    QWidget *embeddedWidget() override;

    double number() const;

private:
    QLabel *_label;

    double _number;
};
