#include "NodePorts.hpp"

#include <stdexcept>
#include <utility>

namespace QtNodes {

NodePorts::NodePorts() {}

NodePorts::~NodePorts() {}

unsigned int NodePorts::nPorts(PortType portType) const
{
    switch (portType) {
    case PortType::In:
        return (unsigned int) _inputPorts.size();
    case PortType::Out:
        return (unsigned int) _outputPorts.size();
    default:
        break;
    }
    return 0;
}

void NodePorts::createPort(PortType portType,
                           std::shared_ptr<NodeData> nodeData,
                           const PortCaption name,
                           ConnectionPolicy policy)
{
    if (nodeData) {
        NodePort port = {nodeData, name, policy};

        if (name.isEmpty())
            port.name = (QString) nodeData->type();

        switch (portType) {
        case PortType::In:
            _inputPorts.push_back(port);
            break;

        case PortType::Out:
            _outputPorts.push_back(port);
            break;

        default:
            return;
        }
    }
}

void NodePorts::insertPort(PortType portType,
                           PortIndex portIndex,
                           std::shared_ptr<NodeData> nodeData,
                           const PortCaption name,
                           ConnectionPolicy policy)
{
    if (nodeData) {
        NodePort port = {nodeData, name, policy};

        if (name.isEmpty())
            port.name = (QString) nodeData->type();

        switch (portType) {
        case PortType::In:
            if (portIndex > (unsigned int) _inputPorts.size())
                return;
            _inputPorts.insert(_inputPorts.begin() + portIndex, port);
            break;

        case PortType::Out:
            if (portIndex > (unsigned int) _outputPorts.size())
                return;
            _outputPorts.insert(_outputPorts.begin() + portIndex, port);
            break;

        default:
            return;
        }
    }
}

void NodePorts::removePort(PortType portType, PortIndex portIndex)
{
    switch (portType) {
    case PortType::In:
        if (portIndex >= (unsigned int) _inputPorts.size())
            return;
        _inputPorts.erase(_inputPorts.begin() + portIndex);
        break;

    case PortType::Out:
        if (portIndex >= (unsigned int) _inputPorts.size())
            return;
        _outputPorts.erase(_outputPorts.begin() + portIndex);
        break;

    default:
        return;
    }
}

NodePort const &NodePorts::port(PortType portType, PortIndex portIndex) const
{
    try {
        switch (portType) {
        case PortType::In:
            return _inputPorts.at(portIndex);

        case PortType::Out:
            return _outputPorts.at(portIndex);

        default:
            break;
        }
    } catch (...) {
    }
    throw std::out_of_range("No port found");
}

std::shared_ptr<NodeData> NodePorts::portData(PortType portType, PortIndex portIndex) const
{
    return port(portType, portIndex).data;
}

void NodePorts::setPortData(PortType portType,
                            PortIndex portIndex,
                            std::shared_ptr<NodeData> nodeData)
{
    NodePort &p = const_cast<NodePort &>(port(portType, portIndex));
    if (p.data)
        p.data = std::move(nodeData);
}

PortCaption NodePorts::portCaption(PortType portType, PortIndex portIndex) const
{
    return port(portType, portIndex).name;
}

void NodePorts::setPortCaption(PortType portType, PortIndex portIndex, const PortCaption name)
{
    NodePort &p = const_cast<NodePort &>(port(portType, portIndex));
    if (p.data)
        p.name = name;
}

ConnectionPolicy NodePorts::portConnectionPolicy(PortType portType, PortIndex portIndex) const
{
    return port(portType, portIndex).connectionPolicy;
}

void NodePorts::setPortConnectionPolicy(PortType portType,
                                        PortIndex portIndex,
                                        ConnectionPolicy policy)
{
    NodePort &p = const_cast<NodePort &>(port(portType, portIndex));
    if (p.data)
        p.connectionPolicy = policy;
}

} // namespace QtNodes