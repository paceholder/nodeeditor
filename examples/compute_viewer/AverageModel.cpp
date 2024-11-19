#include "AverageModel.hpp"
#include "AverageData.hpp"
#include "RoiData.hpp"
#include <QtNodes/NodeDelegateModel>

using QtNodes::NodeStyle;

unsigned int AverageModel::nPorts(PortType portType) const
{
    unsigned int result;

    if (portType == PortType::In)
        result = 1;
    else
        result = 1;

    return result;
}

NodeDataType AverageModel::dataType(PortType portType, PortIndex) const
{
    if (portType == PortType::In) {
        return RoiData().type();
    } else {
        return AverageData().type();
    }
}

std::shared_ptr<NodeData> AverageModel::outData(PortIndex)
{
    return nullptr;
}

void AverageModel::setInData(std::shared_ptr<NodeData> data, PortIndex portIndex) {}

QWidget *AverageModel::embeddedWidget()
{
    return nullptr;
}
