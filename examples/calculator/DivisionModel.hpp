#pragma once

#include "DecimalData.hpp"
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

        setPortCaption(PortType::In, 0, "Dividend");
        setPortCaption(PortType::In, 1, "Divisor");
        setPortCaption(PortType::Out, 0, "Result");
    }

    QString caption() const override { return QStringLiteral("Division"); }

    QString name() const override { return QStringLiteral("Division"); }

private:
    void compute() override
    {
        PortIndex const outPortIndex = 0;

        auto n1 = _number1.lock();
        auto n2 = _number2.lock();

        if (n2 && (n2->number() == 0.0)) {
            //modelValidationState = NodeValidationState::Error;
            //modelValidationError = QStringLiteral("Division by zero error");
            _result.reset();
        } else if (n1 && n2) {
            //modelValidationState = NodeValidationState::Valid;
            //modelValidationError = QString();
            setPortData(PortType::Out,
                        0,
                        std::make_shared<DecimalData>(n1->number() / n2->number()));
        } else {
            //modelValidationState = NodeValidationState::Warning;
            //modelValidationError = QStringLiteral("Missing or incorrect inputs");
            _result.reset();
        }

        Q_EMIT dataUpdated(outPortIndex);
    }
};
