#include "MathOperationDataModel.hpp"

#include "DecimalData.hpp"

void MathOperationDataModel::init()
{
    createPort(PortType::In, std::make_shared<DecimalData>(), "Decimal");
    createPort(PortType::In, std::make_shared<DecimalData>(), "Decimal");

    _result = std::make_shared<DecimalData>();
    createPort(PortType::Out, _result, "Decimal", QtNodes::ConnectionPolicy::Many);
}

void MathOperationDataModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex)
{
    auto numberData = std::dynamic_pointer_cast<DecimalData>(data);

    if (!data) {
        Q_EMIT dataInvalidated(0);
    }

    if (portIndex == 0) {
        _number1 = numberData;
    } else {
        _number2 = numberData;
    }

    compute();
}
