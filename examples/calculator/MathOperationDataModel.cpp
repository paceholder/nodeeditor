#include "MathOperationDataModel.hpp"

void MathOperationDataModel::init()
{
    createPort(PortType::In, "decimal", "Decimal");
    createPort(PortType::In, "decimal", "Decimal");

    createPort(PortType::Out, "decimal", "Decimal", QtNodes::ConnectionPolicy::Many);
}

void MathOperationDataModel::setInData(QVariant const nodeData, PortIndex portIndex)
{
    if (nodeData.isNull()) {
        Q_EMIT dataInvalidated(0);
        return;
    }

    if (portIndex == 0) {
        _number1 = nodeData.toDouble();
    } else {
        _number2 = nodeData.toDouble();
    }

    compute();
}
