#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <QtNodes/NodeDelegateModel>

#include "MathOperationDataModel.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class SubtractionModel : public MathOperationDataModel
{
public:
    virtual ~SubtractionModel() {}

public:
    void init() override
    {
        MathOperationDataModel::init();

        updatePortCaption(PortType::In, 0, "Minuend");
        updatePortCaption(PortType::In, 1, "Subtracted");
        updatePortCaption(PortType::Out, 0, "Result");
    }

    QString caption() const override { return QStringLiteral("Subtraction"); }

    QString name() const override { return QStringLiteral("Subtraction"); }

private:
    void compute() override
    {
        PortIndex const outPortIndex = 0;

        updateOutPortData(outPortIndex, _number1 - _number2);

        Q_EMIT dataUpdated(outPortIndex);
    }
};
