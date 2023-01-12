#include "NodeDelegateModel.hpp"

#include "StyleCollection.hpp"

#include <utility>

namespace QtNodes {

NodeDelegateModel::NodeDelegateModel()
    : _nodeStyle(StyleCollection::nodeStyle())
{
    // Derived classes can initialize specific style here
}

QJsonObject NodeDelegateModel::save() const
{
    QJsonObject modelJson;

    modelJson["model-name"] = name();

    return modelJson;
}

void NodeDelegateModel::load(QJsonObject const &)
{
    //
}

NodeStyle const &NodeDelegateModel::nodeStyle() const
{
    return _nodeStyle;
}

void NodeDelegateModel::setNodeStyle(NodeStyle const &style)
{
    _nodeStyle = style;
}

#pragma region /* port */

unsigned int NodeDelegateModel::nPorts(PortType portType) const
{
    switch (portType) {
    case PortType::In:
        return (unsigned int) _inputPorts.size();
    case PortType::Out:
        return (unsigned int) _outputPorts.size();
    }
    return 0;
}

void NodeDelegateModel::createPort(PortType portType,
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
        }
    }
}

NodePort const &NodeDelegateModel::port(PortType portType, PortIndex portIndex) const
{
    try {
        switch (portType) {
        case PortType::In: {
            return _inputPorts.at(portIndex);
        }

        case PortType::Out: {
            return _outputPorts.at(portIndex);
        }
        }
    } catch (...) {
    }
    throw std::out_of_range("No port found");
}

std::shared_ptr<NodeData> NodeDelegateModel::portData(PortType portType, PortIndex portIndex) const
{
    return port(portType, portIndex).data;
}

void NodeDelegateModel::setPortData(PortType portType,
                                    PortIndex portIndex,
                                    std::shared_ptr<NodeData> nodeData)
{
    NodePort &p = const_cast<NodePort &>(port(portType, portIndex));
    if (p.data)
        p.data = std::move(nodeData);
}

PortCaption NodeDelegateModel::portCaption(PortType portType, PortIndex portIndex) const
{
    return port(portType, portIndex).name;
}

void NodeDelegateModel::setPortCaption(PortType portType,
                                       PortIndex portIndex,
                                       const PortCaption name)
{
    NodePort &p = const_cast<NodePort &>(port(portType, portIndex));
    if (p.data)
        p.name = name;
}

ConnectionPolicy NodeDelegateModel::portConnectionPolicy(PortType portType,
                                                         PortIndex portIndex) const
{
    return port(portType, portIndex).connectionPolicy;
}

void NodeDelegateModel::setPortConnectionPolicy(PortType portType,
                                                PortIndex portIndex,
                                                ConnectionPolicy policy)
{
    NodePort &p = const_cast<NodePort &>(port(portType, portIndex));
    if (p.data)
        p.connectionPolicy = policy;
}

#pragma endregion /* port */

} // namespace QtNodes
