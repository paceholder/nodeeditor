#include "DataFlowModel.hpp"
#include "data_models/UIBufferBase.hpp"

DataFlowModel::DataFlowModel(std::shared_ptr<NodeDelegateModelRegistry> registry)
    : DataFlowGraphModel(std::move(registry))

{}

void DataFlowModel::setFloatingProperties(QPointer<FloatingProperties> propertyPanel)
{
    _propertyPanel = propertyPanel;
}

NodeId DataFlowModel::addNode(QString const nodeType)
{
    if (nodeType == "VideoInput") {
        auto it = nodesMap.find(nodeType);
        if (it != nodesMap.end()) {
            const std::unordered_set<NodeId> &nodeSet = it->second;
            if (nodeSet.size() > 0) {
                return InvalidNodeId;
            }
        }
    }
    NodeId newNodeId = DataFlowGraphModel::addNode(nodeType);
    if (nodeType == "Process") {
        _nodePortCounts[newNodeId].in = 0;
        _nodePortCounts[newNodeId].out = 0;
        _processNodeSize[newNodeId] = QSize(300, 120);
        Q_EMIT nodeUpdated(newNodeId);
    }
    _nodeNames[newNodeId] = QString(nodeType);
    OperationDataModel *nodeModel = delegateModel<OperationDataModel>(newNodeId);
    nodeModel->setNodeId(newNodeId);

    if (nodeModel) {
        nodeModel->setNodeName(generateNewNodeName(nodeType));
        nodesMap[nodeType].insert(newNodeId);
    }
    return newNodeId;
}

bool DataFlowModel::deleteNode(NodeId const nodeId)
{
    QVariant nodeTypeName = nodeData(nodeId, QtNodes::NodeRole::Type);
    if (nodeTypeName == "VideoInput") {
        return false;
    } else if (nodeTypeName == "VideoOutput") {
        auto it = nodesMap.find(nodeTypeName.toString());
        if (it != nodesMap.end()) {
            const std::unordered_set<NodeId> &nodeSet = it->second;
            if (nodeSet.size() == 1) {
                return false;
            }
        }
    }
    _nodeWidgets.erase(nodeId);
    _nodePortCounts.erase(nodeId);
    auto it = nodesMap.find(nodeTypeName.toString());
    if (it != nodesMap.end()) {
        it->second.erase(nodeId);
    }
    _propertyPanel->unsetObject();
    return DataFlowGraphModel::deleteNode(nodeId);
}

PortAddRemoveWidget *DataFlowModel::widget(NodeId nodeId) const
{
    auto it = _nodeWidgets.find(nodeId);
    if (it == _nodeWidgets.end()) {
        _nodeWidgets[nodeId] = new PortAddRemoveWidget(nodeId, *const_cast<DataFlowModel *>(this));
        _nodeWidgets[nodeId]->resize(10, 10);
    }

    return _nodeWidgets[nodeId];
}

QVariant DataFlowModel::nodeData(NodeId nodeId, NodeRole role) const
{
    QVariant nodeTypeName = DataFlowGraphModel::nodeData(nodeId, QtNodes::NodeRole::Type);
    if (nodeTypeName.toString() == "Process") {
        switch (role) {
        case NodeRole::PortOffset:
            return 35;
        case NodeRole::Size:
            return _processNodeSize[nodeId];
        case NodeRole::InPortCount:
            return _nodePortCounts[nodeId].in;

        case NodeRole::OutPortCount:
            return _nodePortCounts[nodeId].out;

        case NodeRole::Widget:
            return QVariant::fromValue(widget(nodeId));
        }
    }
    return DataFlowGraphModel::nodeData(nodeId, role);
}

bool DataFlowModel::setNodeData(NodeId nodeId, NodeRole role, QVariant value)
{
    QVariant nodeTypeName = DataFlowGraphModel::nodeData(nodeId, QtNodes::NodeRole::Type);
    if (nodeTypeName == "Process") {
        if (role == NodeRole::Position) {
            QPointF pos = value.value<QPointF>();
            QPair<float, float> nodeRange = getProcessNodeRange(nodeId, pos);
            QSize size = _processNodeSize[nodeId];
            if ((pos.x() < nodeRange.first) || ((pos.x() + size.width()) > nodeRange.second))
                return false;
        }
        switch (role) {
        case NodeRole::Size:
            _processNodeSize[nodeId] = value.value<QSize>();
            _processNodeSize[nodeId].setHeight(_processNodeSize[nodeId].height() + 15);
            return true;
        case NodeRole::InPortCount:
            _nodePortCounts[nodeId].in = value.toUInt();
            return false;

        case NodeRole::OutPortCount:
            _nodePortCounts[nodeId].out = value.toUInt();
            return false;

        case NodeRole::Widget:
            return false;
        }
    }

    return DataFlowGraphModel::setNodeData(nodeId, role, value);
}

void DataFlowModel::addProcessNodePort(NodeId nodeId,
                                       PortType portType,
                                       PortIndex portIndex,
                                       PortBase *port)
{
    PortIndex first = portIndex + 1;
    PortIndex last = first;
    portsAboutToBeInserted(nodeId, portType, first, last);
    Process *nodeModel = delegateModel<Process>(nodeId);

    if (portType == PortType::In) {
        _nodePortCounts[nodeId].in++;
        nodeModel->setPortTypeLeft(_nodePortCounts[nodeId].in, port);
    } else {
        _nodePortCounts[nodeId].out++;
        nodeModel->setPortTypeRight(_nodePortCounts[nodeId].out, port);
    }

    portsInserted();

    Q_EMIT nodeUpdated(nodeId);
}

void DataFlowModel::removeProcessNodePort(NodeId nodeId, PortType portType, PortIndex portIndex)
{
    PortIndex first = portIndex;
    PortIndex last = first;
    portsAboutToBeDeleted(nodeId, portType, first, last);
    Process *nodeModel = delegateModel<Process>(nodeId);
    if (portType == PortType::In) {
        nodeModel->removePortTypeLeft(portIndex);
        _nodePortCounts[nodeId].in--;
    } else {
        nodeModel->removePortTypeRight(portIndex);
        _nodePortCounts[nodeId].out--;
    }

    portsDeleted();

    Q_EMIT nodeUpdated(nodeId);
}

QString DataFlowModel::generateNewNodeName(QString typeNamePrefix)
{
    auto it = nodesMap.find(typeNamePrefix);
    int nodeCount = 0;
    if (it != nodesMap.end()) {
        const std::unordered_set<NodeId> &nodeSet = it->second;
        nodeCount = nodeSet.size();
    }
    return typeNamePrefix + QString("%1").arg(nodeCount + 1, 3, 10, QChar('0'));
}

float DataFlowModel::getlastProcessLeft()
{
    auto it = nodesMap.find("Process");
    int nodeCount = 0;
    if (it != nodesMap.end()) {
        const std::unordered_set<NodeId> &nodeSet = it->second;
        nodeCount = nodeSet.size();
        if (nodeCount == 0)
            return -2;
        int lastNodeRight = std::numeric_limits<int>::min();
        for (const NodeId &node : nodeSet) {
            QPointF position = nodeData(node, NodeRole::Position).value<QPointF>();
            QSize size = nodeData(node, NodeRole::Size).value<QSize>();
            float nodeRight = position.x() + size.width();
            if (nodeRight > lastNodeRight) {
                lastNodeRight = nodeRight;
            }
        }
        return lastNodeRight;
    }
    return -2;
}

QPair<float, float> DataFlowModel::getProcessNodeRange(NodeId nodeId, QPointF currentPos)
{
    QSize size = _processNodeSize[nodeId];
    float currentNodeCenterX = currentPos.x() + size.width() / 2;
    auto it = nodesMap.find("Process");
    if (it != nodesMap.end()) {
        const std::unordered_set<NodeId> &nodeSet = it->second;
        NodeId leftNeighbor = InvalidNodeId;
        NodeId rightNeighbor = InvalidNodeId;
        QPair<float, float> leftNeighborRange = qMakePair(std::numeric_limits<int>::min(),
                                                          std::numeric_limits<int>::min());
        QPair<float, float> rightNeighborRange = qMakePair(std::numeric_limits<int>::max(),
                                                           std::numeric_limits<int>::max());

        for (const NodeId &node : nodeSet) {
            if (node == nodeId)
                continue;
            QPointF position = nodeData(node, NodeRole::Position).value<QPointF>();
            QSize size = _processNodeSize[node];
            float nodeLeft = position.x();
            float nodeRight = position.x() + size.width();
            if (nodeLeft < currentNodeCenterX) { //left
                if (nodeLeft > leftNeighborRange.first) {
                    leftNeighborRange.first = nodeLeft;
                    leftNeighborRange.second = nodeRight;
                }
            } else { // right
                if (nodeLeft < rightNeighborRange.first) {
                    rightNeighborRange.first = nodeLeft;
                    rightNeighborRange.second = nodeRight;
                }
            }
        }
        return qMakePair(leftNeighborRange.second + 25, rightNeighborRange.first - 25);
    }
    return qMakePair(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
}

void DataFlowModel::setSelectedNode(OperationDataModel *node, NodeId nodeId)
{
    if (nodeData(nodeId, QtNodes::NodeRole::Type).toString() == "Process") {
        PortAddRemoveWidget *nodeWidget = widget(nodeId);
        nodeWidget->rootSelected();
    }
}

void DataFlowModel::deselectNode() {}

void DataFlowModel::setSelectedPort(NodeId nodeId, bool isRightPort, int portIndex)
{
    Process *nodeModel = delegateModel<Process>(nodeId);
    if (nodeModel) {
        emit nodePortSelected(isRightPort, nodeModel, portIndex);
    }
}

void DataFlowModel::notifyPortInsertion(NodeId nodeId)
{
    portsInserted();

    Q_EMIT nodeUpdated(nodeId);
}

void DataFlowModel::addProcessPort(NodeId nodeId, bool isRight, bool isImage)
{
    Process *nodeModel = delegateModel<Process>(nodeId);
    if (!nodeModel)
        return;
    if (isRight) {
        nodeModel->addOutput(this, isImage, "output001");
    } else {
        nodeModel->addInput(this, isImage, "input001");
    }
}

void DataFlowModel::addConnection(ConnectionId const connectionId)
{
    DataFlowGraphModel::addConnection(connectionId);
    Process *inProcessNode = delegateModel<Process>(connectionId.inNodeId);
    if (inProcessNode != nullptr) {
        UIBufferBase *bufferNode = delegateModel<UIBufferBase>(connectionId.outNodeId);
        if (bufferNode)
            inProcessNode->addInPortConnection(bufferNode, connectionId.inPortIndex);
    } else {
        Process *outProcessNode = delegateModel<Process>(connectionId.outNodeId);
        if (outProcessNode != nullptr) {
            UIBufferBase *bufferNode = delegateModel<UIBufferBase>(connectionId.inNodeId);
            if (bufferNode)
                inProcessNode->addOutPortConnection(bufferNode, connectionId.outPortIndex);
        }
    }
}

bool DataFlowModel::deleteConnection(ConnectionId const connectionId)
{
    DataFlowGraphModel::deleteConnection(connectionId);
    Process *inProcessNode = delegateModel<Process>(connectionId.inNodeId);
    if (inProcessNode != nullptr) {
        UIBufferBase *bufferNode = delegateModel<UIBufferBase>(connectionId.outNodeId);
        if (bufferNode)
            inProcessNode->removeInPortConnection(connectionId.inPortIndex);
    } else {
        Process *outProcessNode = delegateModel<Process>(connectionId.outNodeId);
        if (outProcessNode != nullptr) {
            UIBufferBase *bufferNode = delegateModel<UIBufferBase>(connectionId.inNodeId);
            if (bufferNode)
                inProcessNode->removeOutPortConnection(connectionId.outPortIndex);
        }
    }
}