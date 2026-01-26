#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QTimer>
#include <QtCore/QObject>
#include <QtCore/QElapsedTimer>
#include <QtWidgets/QLabel>
#include <QtCore/QRandomGenerator64>

#include "MathOperationDataModel.hpp"
#include "DecimalData.hpp"

/// The model generates a random value in a long processing schema, as it should demonstrate
/// the usage of the NodeProcessingStatus and the ProgressValue functionality.
/// The random number is generate in the [n1, n2] interval.
class RandomNumberModel : public MathOperationDataModel
{
public:
    RandomNumberModel() {
        this->setNodeProcessingStatus(QtNodes::NodeProcessingStatus::Empty);


        QObject::connect(this, &NodeDelegateModel::computingStarted, this, [this]() {
            this->setNodeProcessingStatus(
                QtNodes::NodeProcessingStatus::Processing);

            setProgressValue(QString{"0%"});
            emit requestNodeUpdate();

            _elapsedTimer.start();

            if (!_progressTimer) {
                _progressTimer = new QTimer(this);
                connect(_progressTimer, &QTimer::timeout, this, [this]() {
                    qint64 elapsed = _elapsedTimer.elapsed();
                    int percent = static_cast<int>((double(elapsed) / _totalDurationMs) * 100.0);

                    if (percent > 100)
                        percent = 100;

                    setProgressValue(QString::number(percent) + "%");
                    emit requestNodeUpdate();
                });
            }

            _progressTimer->start(_progressUpdateIntervalMs);

            emit requestNodeUpdate();
        });

        QObject::connect(this, &NodeDelegateModel::computingFinished, this, [this]() {
            if (_progressTimer) {
                _progressTimer->stop();
            }

            setProgressValue(QString());

            this->setNodeProcessingStatus(
                QtNodes::NodeProcessingStatus::Updated);

            emit requestNodeUpdate();
        });
    }

    virtual ~RandomNumberModel() {
        if (_progressTimer) {
            _progressTimer->stop();
            delete _progressTimer;
        }
    }

public:
    QString caption() const override { return QStringLiteral("Random Number"); }

    QString name() const override { return QStringLiteral("Random Number"); }

private:
    void compute() override
    {
        auto n1 = _number1.lock();
        auto n2 = _number2.lock();

        if (!n1 || !n2) {
            return;
        }

        Q_EMIT computingStarted();
        PortIndex const outPortIndex = 0;

        QTimer::singleShot(_totalDurationMs, this, [this, n1, n2]() {
            if (n1 && n2) {
                double a = n1->number();
                double b = n2->number();

                if (a > b) {
                    setNodeProcessingStatus(QtNodes::NodeProcessingStatus::Failed);

                    if (_progressTimer) {
                        _progressTimer->stop();
                    }

                    setProgressValue(QString());

                    emit requestNodeUpdate();
                    return;
                }

                double upper = std::nextafter(b, std::numeric_limits<double>::max());
                double randomValue = QRandomGenerator::global()->generateDouble() * (upper - a) + a;

                _result = std::make_shared<DecimalData>(randomValue);
                emit computingFinished();
            } else {
                _result.reset();
            }

            Q_EMIT dataUpdated(outPortIndex);
        });
    }

    QTimer *_progressTimer = nullptr;
    QElapsedTimer _elapsedTimer;

    const int _totalDurationMs = 3000;
    const int _progressUpdateIntervalMs = 50;
};
