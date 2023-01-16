#pragma once

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include "MathOperationDataModel.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MultiplicationModel : public MathOperationDataModel
{
public:
    virtual ~MultiplicationModel() {}

public:
    QString caption() const override { return QStringLiteral("Multiplication"); }

    QString name() const override { return QStringLiteral("Multiplication"); }

private:
    void compute() override
    {
        PortIndex const outPortIndex = 0;

        updateOutPortData(outPortIndex, _number1 * _number2);

        Q_EMIT dataUpdated(outPortIndex);
    }
};
