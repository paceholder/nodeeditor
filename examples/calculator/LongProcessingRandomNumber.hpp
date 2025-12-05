#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QTimer>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator64>

#include "MathOperationDataModel.hpp"
#include "DecimalData.hpp"

/// The model generates a random value in a long processing schema,
/// as it should demonstrate the usage of the NodeProcessingStatus.
/// The random number is generate in the [n1, n2] interval.
class RandomNumberModel : public MathOperationDataModel
{
public:
    RandomNumberModel() {
        this->setNodeProcessingStatus(QtNodes::NodeProcessingStatus::Empty);


        QObject::connect(this, &NodeDelegateModel::computingStarted, this, [this]() {
            if (_number1.lock() && _number2.lock()) {
                this->setNodeProcessingStatus(
                    QtNodes::NodeProcessingStatus::Processing);
            }

            emit requestNodeUpdate();
        });
        QObject::connect(this, &NodeDelegateModel::computingFinished, this, [this]() {
            this->setNodeProcessingStatus(
                QtNodes::NodeProcessingStatus::Updated);

            emit requestNodeUpdate();
        });
    }
    virtual ~RandomNumberModel() {}

public:
    QString caption() const override { return QStringLiteral("Random Number"); }

    QString name() const override { return QStringLiteral("Random Number"); }

private:
    void compute() override
    {
        Q_EMIT computingStarted();
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

                    if (a > b) {
                        setNodeProcessingStatus(QtNodes::NodeProcessingStatus::Failed);

                        emit requestNodeUpdate();
                        return;
                    }

                    double upper = std::nextafter(b, std::numeric_limits<double>::max());
                    double randomValue = QRandomGenerator::global()->generateDouble() * (upper - a) + a;

                    _result = std::make_shared<DecimalData>(randomValue);
                    Q_EMIT computingFinished();
                } else {
                    _result.reset();
                }

                Q_EMIT dataUpdated(outPortIndex);
            }
        });
    }
};
