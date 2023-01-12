#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QLabel>

#include <QtNodes/NodeDelegateModel>

#include "MathOperationDataModel.hpp"

#include "DecimalData.hpp"

/// The model dictates the number of inputs and outputs for the Node.
/// In this example it has no logic.
class SubtractionModel : public MathOperationDataModel
{
public:
    virtual ~SubtractionModel() {}

public:
    void init() override
    {
        MathOperationDataModel::init();

        setPortCaption(PortType::In, 0, "Minuend");
        setPortCaption(PortType::In, 1, "Subtracted");
        setPortCaption(PortType::Out, 0, "Result");
    }

    QString caption() const override { return QStringLiteral("Subtraction"); }

    QString name() const override { return QStringLiteral("Subtraction"); }

private:
    void compute() override
    {
        PortIndex const outPortIndex = 0;

        auto n1 = _number1.lock();
        auto n2 = _number2.lock();

        if (n1 && n2) {
            setPortData(PortType::Out,
                        0,
                        std::make_shared<DecimalData>(n1->number() - n2->number()));
        } else {
            _result.reset();
        }

        Q_EMIT dataUpdated(outPortIndex);
    }
};
