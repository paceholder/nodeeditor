#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QTimer>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <random>

#include "MathOperationDataModel.hpp"
#include "DecimalData.hpp"

/// The model generates a random value in a long processing schema,
/// as it should demonstrate the usage of the NodeProcessingStatus.
/// The random number is generate in the [n1, n2] interval.
class RandomNumberModel : public MathOperationDataModel
{
public:
    virtual ~RandomNumberModel() {}

public:
    QString caption() const override { return QStringLiteral("Random Number"); }

    QString name() const override { return QStringLiteral("Random Number"); }

    bool labelEditable() const override { return true; }

private:
    void compute() override
    {
        PortIndex const outPortIndex = 0;

        auto n1 = _number1.lock();
        auto n2 = _number2.lock();

        QTimer *timer = new QTimer(this);
        timer->start(1000);
        int secondsRemaining = 3;
        connect(timer, &QTimer::timeout, this, [=]() mutable {
            if (--secondsRemaining <= 0) {
                timer->stop();
                if (n1 && n2) {
                    double a = n1->number();
                    double b = n2->number();

                    double minVal = std::min(a, b);
                    double maxVal = std::max(a, b);

                    std::random_device rd;
                    std::mt19937 gen(rd());

                    std::uniform_real_distribution<double> dist(minVal, maxVal);

                    double randomValue = dist(gen);

                    _result = std::make_shared<DecimalData>(randomValue);
                } else {
                    _result.reset();
                }

                Q_EMIT dataUpdated(outPortIndex);
            }
        });
    }
};
