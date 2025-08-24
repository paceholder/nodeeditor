#pragma once

#include "DecimalData.hpp"
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

        auto n1 = _number1.lock();
        auto n2 = _number2.lock();

        if (n1 && n2) {
            _result = std::make_shared<DecimalData>(n1->number() + n2->number());
        } else {
            _result.reset();
        }

        Q_EMIT dataUpdated(outPortIndex);
    }
};
