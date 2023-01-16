#pragma once

#include "MathOperationDataModel.hpp"

#include <QtNodes/NodeDelegateModel>

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class DivisionModel : public MathOperationDataModel
{
public:
    virtual ~DivisionModel() {}

public:
    void init() override
    {
        MathOperationDataModel::init();

        updatePortCaption(PortType::In, 0, "Dividend");
        updatePortCaption(PortType::In, 1, "Divisor");
        updatePortCaption(PortType::Out, 0, "Result");
    }

    QString caption() const override { return QStringLiteral("Division"); }

    QString name() const override { return QStringLiteral("Division"); }

private:
    void compute() override
    {
        PortIndex const outPortIndex = 0;

        updateOutPortData(outPortIndex, _number1 / _number2);

        Q_EMIT dataUpdated(outPortIndex);
    }
};
