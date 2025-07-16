#pragma once

#include <QtNodes/NodeDelegateModel>
#include <QtNodes/BasicGraphicsScene>
#include <QTimer>
#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include "MathOperationDataModel.hpp"
#include "DecimalData.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class MultiplicationModel : public MathOperationDataModel
{
public:
    MultiplicationModel() {
        /*
        QObject::connect(this, &NodeDelegateModel::computingStarted, this, [this]() {
            this->setNodeProcessingStatus(
                NodeDelegateModel::NodeProcessingStatus::Status::Processing);
        });
        QObject::connect(this, &NodeDelegateModel::computingFinished, this, [this]() {
            this->setNodeProcessingStatus(
                NodeDelegateModel::NodeProcessingStatus::Status::Updated);
        });
*/
    }
    virtual ~MultiplicationModel() {}

public:
    QString caption() const override { return QStringLiteral("Multiplication"); }

    QString name() const override { return QStringLiteral("Multiplication"); }

private:
    void compute() override
    {
        PortIndex const outPortIndex = 0;

        auto n1 = _number1.lock();
        auto n2 = _number2.lock();

        // Dentro da sua classe:
        QTimer *timer = new QTimer(this);
        timer->start(1000); // 1000ms = 1 segundo
        int secondsRemaining = 3;
        connect(timer, &QTimer::timeout, this, [=]() mutable {
            if (--secondsRemaining <= 0) {
                timer->stop();
                if (n1 && n2) {
                    //modelValidationState = NodeValidationState::Valid;
                    //modelValidationError = QString();
                    _result = std::make_shared<DecimalData>(n1->number() * n2->number());
                } else {
                    //modelValidationState = NodeValidationState::Warning;
                    //modelValidationError = QStringLiteral("Missing or incorrect inputs");
                    _result.reset();
                }

                Q_EMIT dataUpdated(outPortIndex);
                // Faça algo quando o timer acabar
            }
            // Atualize sua UI aqui, ex: label->setText(QString::number(secondsRemaining));
        });
    }
};
