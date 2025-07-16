#include "DataFlowModel.hpp"

DataFlowModel::DataFlowModel(std::shared_ptr<NodeDelegateModelRegistry> registry)
    : DataFlowGraphModel(std::move(registry))
{}

NodeId DataFlowModel::addNode(QString const nodeType)
{
    if (nodeType == "VideoOutput") {
        auto it = nodesMap.find(NodeTypes::Video_Output);
        if (it != nodesMap.end()) {
            const std::unordered_set<NodeId> &nodeSet = it->second;
            if (nodeSet.size() > 0) {
                return InvalidNodeId;
            }
        }
    }
    NodeId newNodeId = DataFlowGraphModel::addNode(nodeType);
    if (nodeType == "Process") {
        _nodePortCounts[newNodeId].in = 1;
        widget(newNodeId)->populateButtons(PortType::In, 1);
        _nodePortCounts[newNodeId].out = 1;
        widget(newNodeId)->populateButtons(PortType::Out, 1);
        _nodeSize[newNodeId] = QSize(250, 130);
    }
    _nodeNames[newNodeId]=QString(nodeType) ;// + newNodeId;
    return newNodeId;
}

NodeId DataFlowModel::addNodeType(NodeTypes type)
{
    QString nodeTypeName = nodeTypeToName[type];
    NodeId newNodeId = addNode(nodeTypeName);
    nodesMap[type].insert(newNodeId);
    return newNodeId;
}

NodeId DataFlowModel::addNodeName(QString const nodeTypeName)
{
    auto nodeType = stringToNodeType(nodeTypeName);
    if (nodeType) {
        NodeTypes type = *nodeType;
        return addNodeType(type);
    }
    return QtNodes::InvalidNodeId;
}

bool DataFlowModel::deleteNode(NodeId const nodeId)
{
    QVariant nodeTypeName = nodeData(nodeId, QtNodes::NodeRole::Type);
    auto nodeType = stringToNodeType(nodeTypeName.toString());
    if (nodeTypeName == "VideoOutput") {
        return false;
    } else if (nodeTypeName == "VideoInput") {
        if (nodeType) {
            NodeTypes type = *nodeType;
            auto it = nodesMap.find(type);
            if (it != nodesMap.end()) {
                const std::unordered_set<NodeId> &nodeSet = it->second;
                if (nodeSet.size() == 1) {
                    return false;
                }
            }
        }
    }
    _nodeWidgets.erase(nodeId);
    _nodePortCounts.erase(nodeId);
    if (nodeType) {
        NodeTypes type = *nodeType;
        auto it = nodesMap.find(type);
        if (it != nodesMap.end()) {
            it->second.erase(nodeId);
        }
    }

    return DataFlowGraphModel::deleteNode(nodeId);
}

std::optional<NodeTypes> DataFlowModel::stringToNodeType(const QString &str) const
{
    static const QHash<QString, NodeTypes> map = [] {
        QHash<QString, NodeTypes> m;
        for (auto it = nodeTypeToName.begin(); it != nodeTypeToName.end(); ++it)
            m[it.value()] = it.key();
        return m;
    }();

    auto it = map.find(str);
    if (it != map.end()) {
        return it.value();
    } else {
        return std::nullopt;
    }
}

PortAddRemoveWidget *DataFlowModel::widget(NodeId nodeId) const
{
    auto it = _nodeWidgets.find(nodeId);
    if (it == _nodeWidgets.end()) {
        _nodeWidgets[nodeId] = new PortAddRemoveWidget(nodeId, *const_cast<DataFlowModel *>(this));
    }

    return _nodeWidgets[nodeId];
}

QVariant DataFlowModel::nodeData(NodeId nodeId, NodeRole role) const
{
    QVariant nodeTypeName = DataFlowGraphModel::nodeData(nodeId, QtNodes::NodeRole::Type);
    if (nodeTypeName == "Process") {
        switch (role) {
        case NodeRole::Size:
            return _nodeSize[nodeId];
        case NodeRole::InPortCount:
            return _nodePortCounts[nodeId].in;

        case NodeRole::OutPortCount:
            return _nodePortCounts[nodeId].out;

        case NodeRole::Widget: {
            return QVariant::fromValue(widget(nodeId));
        }
        }
    }
    return DataFlowGraphModel::nodeData(nodeId, role);
}

bool DataFlowModel::setNodeData(NodeId nodeId, NodeRole role, QVariant value)
{
    QVariant nodeTypeName = DataFlowGraphModel::nodeData(nodeId, QtNodes::NodeRole::Type);
    if (nodeTypeName == "Process") {
        switch (role) {
        case NodeRole::Size:
            _nodeSize[nodeId] = value.value<QSize>();
            return true;
        case NodeRole::InPortCount:
            _nodePortCounts[nodeId].in = value.toUInt();
            widget(nodeId)->populateButtons(PortType::In, value.toUInt());
            return false;

        case NodeRole::OutPortCount:
            _nodePortCounts[nodeId].out = value.toUInt();
            widget(nodeId)->populateButtons(PortType::Out, value.toUInt());
            return false;

        case NodeRole::Widget:
            return false;
        }
    }

    return DataFlowGraphModel::setNodeData(nodeId, role, value);
}

void DataFlowModel::addProcessNodePort(NodeId nodeId, PortType portType, PortIndex portIndex)
{
    PortIndex first = portIndex;
    PortIndex last = first;
    portsAboutToBeInserted(nodeId, portType, first, last);

    if (portType == PortType::In)
        _nodePortCounts[nodeId].in++;
    else
        _nodePortCounts[nodeId].out++;
    portsInserted();

    Q_EMIT nodeUpdated(nodeId);
}

void DataFlowModel::removeProcessNodePort(NodeId nodeId, PortType portType, PortIndex portIndex)
{
    PortIndex first = portIndex;
    PortIndex last = first;
    portsAboutToBeDeleted(nodeId, portType, first, last);

    if (portType == PortType::In)
        _nodePortCounts[nodeId].in--;
    else
        _nodePortCounts[nodeId].out--;

    portsDeleted();

    Q_EMIT nodeUpdated(nodeId);
}