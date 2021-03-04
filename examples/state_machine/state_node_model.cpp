#include "state_node_model.h"

QString StateNodeModel::caption() const
{
    return QStringLiteral("State");
}

QString StateNodeModel::name() const
{
    return QStringLiteral("state_node");
}

unsigned int StateNodeModel::nPorts(PortType portType) const
{
    std::ignore = portType;
    return 1;
}

NodeDataType StateNodeModel::dataType(PortType portType, PortIndex portIndex) const
{
    std::ignore = portType;
    std::ignore = portIndex;
    return getTranstitionType();
}

NodeDataType StateNodeModel::getTranstitionType()
{
    return NodeDataType{"transition_port", ""};
}

QtNodes::NodeDataModel::ConnectionPolicy StateNodeModel::portOutConnectionPolicy(PortIndex) const
{
    return ConnectionPolicy::Many;
}

QtNodes::NodeDataModel::ConnectionPolicy StateNodeModel::portInConnectionPolicy(PortIndex) const
{
    return ConnectionPolicy::Many;
}

void StateNodeModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex port)
{
    std::ignore = nodeData;
    std::ignore = port;
}

void StateNodeModel::setInData(std::vector<std::shared_ptr<NodeData>> nodeData, PortIndex port)
{
    std::ignore = nodeData;
    std::ignore = port;
}

std::shared_ptr<NodeData> StateNodeModel::outData(PortIndex port)
{
    std::ignore = port;
    return nullptr;
}
