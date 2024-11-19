#include "OutputModel.hpp"
#include "CalculatorData.hpp"
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeStyle;

unsigned int OutputModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 0;

    return result;
}

NodeDataType OutputModel::dataType(PortType, PortIndex) const
{
    return CalculatorData().type();
}

std::shared_ptr<NodeData> OutputModel::outData(PortIndex)
{
    return nullptr;
}

void OutputModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {}

QWidget *OutputModel::embeddedWidget()
{
    return nullptr;
}
