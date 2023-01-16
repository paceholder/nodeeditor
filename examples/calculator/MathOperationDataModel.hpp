#pragma once

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QJsonObject>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <iostream>

using QtNodes::NodeDelegateModel;
using QtNodes::PortIndex;
using QtNodes::PortType;

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MathOperationDataModel : public NodeDelegateModel
{
    Q_OBJECT

public:
    ~MathOperationDataModel() = default;

public:
    virtual void init() override;

    void setInData(QVariant const nodeData, PortIndex portIndex) override;

    QWidget *embeddedWidget() override { return nullptr; }

protected:
    virtual void compute() = 0;

protected:
    double _number1;
    double _number2;

    double _result;
};
