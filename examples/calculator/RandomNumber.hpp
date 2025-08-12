#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QTimer>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator64>

#include "MathOperationDataModel.hpp"
#include "DecimalData.hpp"

/// The model generates a random value in a schema produced to
/// demonstrate the usage of the freeze/unfreeze mechanism.
/// The random number is generate in the [n1, n2] interval.
class RandomNumberModel : public MathOperationDataModel
{
public:
    virtual ~RandomNumberModel() {}

public:
    QString caption() const override { return QStringLiteral("Random Number"); }

    QString name() const override { return QStringLiteral("Random Number"); }

private:
    void compute() override
    {
        if (frozen())
            return;

        PortIndex const outPortIndex = 0;

        auto n1 = _number1.lock();
        auto n2 = _number2.lock();

        if (n1 && n2) {
            double a = n1->number();
            double b = n2->number();

            if (a > b)
                std::swap(a, b);

            double randomValue = QRandomGenerator::global()->generateDouble() * (b - a) + a;
            _result = std::make_shared<DecimalData>(randomValue);
        } else {
            _result.reset();
        }

        Q_EMIT dataUpdated(outPortIndex);
    }
};
