#pragma once

#include "MathOperationDataModel.hpp"

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class AdditionModel : public MathOperationDataModel
{
public:
    ~AdditionModel() = default;

public:
    QString caption() const override { return QStringLiteral("Addition"); }

    QString name() const override { return QStringLiteral("Addition"); }

private:
    void compute() override
    {
        PortIndex const outPortIndex = 0;

        updateOutPortData(outPortIndex, _number1 + _number2);

        Q_EMIT dataUpdated(outPortIndex);
    }
};
