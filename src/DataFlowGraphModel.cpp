#include "DataFlowGraphModel.hpp"
#include "ConnectionIdHash.hpp"

#include <QJsonArray>

#include <stdexcept>

namespace QtNodes {

DataFlowGraphModel::DataFlowGraphModel(std::shared_ptr<NodeDelegateModelRegistry> registry)
    : _registry(std::move(registry))
    , _nextNodeId{0}
{}

std::unordered_set<NodeId> DataFlowGraphModel::allNodeIds() const
{
    std::unordered_set<NodeId> nodeIds;
    for_each(_models.begin(), _models.end(), [&nodeIds](auto const &p) { nodeIds.insert(p.first); });

    return nodeIds;
}

std::unordered_set<ConnectionId> DataFlowGraphModel::allConnectionIds(NodeId const nodeId) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&nodeId](ConnectionId const &cid) {
                     return cid.inNodeId == nodeId || cid.outNodeId == nodeId;
                 });

    return result;
}

std::unordered_set<ConnectionId> DataFlowGraphModel::connections(NodeId nodeId,
                                                                 PortType portType,
                                                                 PortIndex portIndex) const
{
    std::unordered_set<ConnectionId> result;

    std::copy_if(_connectivity.begin(),
                 _connectivity.end(),
                 std::inserter(result, std::end(result)),
                 [&portType, &portIndex, &nodeId](ConnectionId const &cid) {
                     return (getNodeId(portType, cid) == nodeId
                             && getPortIndex(portType, cid) == portIndex);
                 });

    return result;
}

bool DataFlowGraphModel::connectionExists(ConnectionId const connectionId) const
{
    return (_connectivity.find(connectionId) != _connectivity.end());
}

NodeId DataFlowGraphModel::addNode(QString const nodeType)
{
    std::unique_ptr<NodeDelegateModel> model = _registry->create(nodeType);

    if (model) {
        model->initialize();

        NodeId newId = newNodeId();

        connect(model.get(),
                &NodeDelegateModel::dataUpdated,
                [newId, this](PortIndex const portIndex) {
                    onOutPortDataUpdated(newId, portIndex);
                });

        connect(model.get(),
                &NodeDelegateModel::portsAboutToBeDeleted,
                this,
                [newId, this](PortType const portType, PortIndex const first, PortIndex const last) {
                    portsAboutToBeDeleted(newId, portType, first, last);
                });

        connect(model.get(),
                &NodeDelegateModel::portsDeleted,
                this,
                &DataFlowGraphModel::portsDeleted);

        connect(model.get(),
                &NodeDelegateModel::portsAboutToBeInserted,
                this,
                [newId, this](PortType const portType, PortIndex const first, PortIndex const last) {
                    portsAboutToBeInserted(newId, portType, first, last);
                });

        connect(model.get(),
                &NodeDelegateModel::portsInserted,
                this,
                &DataFlowGraphModel::portsInserted);

        _models[newId].nodeModel = std::move(model);

        Q_EMIT nodeCreated(newId);

        return newId;
    }

    return InvalidNodeId;
}

bool DataFlowGraphModel::connectionPossible(ConnectionId const connectionId) const
{
    auto getDataType = [&](PortType const portType) {
        return portData(getNodeId(portType, connectionId),
                        portType,
                        getPortIndex(portType, connectionId),
                        PortRole::DataType)
            .value<NodeDataType>();
    };

    auto portVacant = [&](PortType const portType) {
        NodeId const nodeId = getNodeId(portType, connectionId);
        PortIndex const portIndex = getPortIndex(portType, connectionId);
        auto const connected = connections(nodeId, portType, portIndex);

        auto policy = portData(nodeId, portType, portIndex, PortRole::ConnectionPolicyRole)
                          .value<ConnectionPolicy>();

        return connected.empty() || (policy == ConnectionPolicy::Many);
    };

    return getDataType(PortType::Out).id == getDataType(PortType::In).id
           && portVacant(PortType::Out) && portVacant(PortType::In);
}

void DataFlowGraphModel::addConnection(ConnectionId const connectionId)
{
    NodeDataType inType, outType;
    ConnectionPolicy inPolicy = ConnectionPolicy::Many, outPolicy = ConnectionPolicy::Many;

    // Gets the type and policy of the input node
    if (auto inNode = delegateModel<NodeDelegateModel>(connectionId.inNodeId); inNode) {
        inType = inNode->dataType(PortType::In, connectionId.inPortIndex);
        inPolicy = inNode->portConnectionPolicy(PortType::In, connectionId.inPortIndex);
    }

    // Gets the type and policy of the output node
    if (auto outNode = delegateModel<NodeDelegateModel>(connectionId.outNodeId); outNode) {
        outType = outNode->dataType(PortType::Out, connectionId.outPortIndex);
        outPolicy = outNode->portConnectionPolicy(PortType::Out, connectionId.outPortIndex);
    }
    // If the data type is inconsistent, return it directly
    if (inType != outType) return;

    auto connectionExist = [this](NodeId nodeId, PortType portType, int portIndex, ConnectionPolicy policy) {
        return policy == ConnectionPolicy::One && connections(nodeId, portType, portIndex).size() > 0;
    };

    // Check the input and output connection policies
    if (connectionExist(connectionId.inNodeId, PortType::In, connectionId.inPortIndex, inPolicy)) return;
    if (connectionExist(connectionId.outNodeId, PortType::Out, connectionId.outPortIndex, outPolicy)) return;

    // Check if loop connections are made
    if(!dfs(connectionId)) return;

    // There is no assignment of 1, there is ++
    if(_models.at(connectionId.outNodeId).successorNodes.contains(connectionId.inNodeId)) {
        _models.at(connectionId.outNodeId).successorNodes[connectionId.inNodeId]++;
    } else {
        _models.at(connectionId.outNodeId).successorNodes[connectionId.inNodeId] = 1;
    }

    // return;
    _connectivity.insert(connectionId);

    sendConnectionCreation(connectionId);

    QVariant const portDataToPropagate = portData(connectionId.outNodeId,
                                                  PortType::Out,
                                                  connectionId.outPortIndex,
                                                  PortRole::Data);

    setPortData(connectionId.inNodeId,
                PortType::In,
                connectionId.inPortIndex,
                portDataToPropagate,
                PortRole::Data);
}


// Traversing forest using stack structure loop (non-recursive)
/*          000...0
 *              /   \
 *        000..0  00..0
 *           /   \  /   \
 *     000..0     00 0..0
 *
 */
bool DataFlowGraphModel::dfs(ConnectionId const connectionId)
{
    auto& successorNodes = _models.at(connectionId.inNodeId).successorNodes;

    std::stack<NodeId> nodeStack;
     // Prevent duplicate access
    std::unordered_set<NodeId> visitedNodes;

    // Put the initial node on the stack
    for (const auto& node : successorNodes) {
        nodeStack.push(node.first);
    }

    while (!nodeStack.empty()) {
        NodeId nodeId = nodeStack.top();
        nodeStack.pop();

        // If the target node is found, it is a directed ring graph
        if (nodeId == connectionId.outNodeId) {
            return false;
        }

        // If the node has been accessed, skip it
        if (visitedNodes.find(nodeId) != visitedNodes.end()) {
            continue;
        }

        visitedNodes.insert(nodeId);

        // Gets the successor to this node and pushes it onto the stack
        auto& nodes = _models.at(nodeId).successorNodes;
        for (const auto& nextNode : nodes) {
            nodeStack.push(nextNode.first);
        }
    }
    return true;;
}

void DataFlowGraphModel::sendConnectionCreation(ConnectionId const connectionId)
{
    Q_EMIT connectionCreated(connectionId);

    auto iti = _models.find(connectionId.inNodeId);
    auto ito = _models.find(connectionId.outNodeId);
    if (iti != _models.end() && ito != _models.end()) {
        auto &modeli = iti->second.nodeModel;
        auto &modelo = ito->second.nodeModel;
        modeli->inputConnectionCreated(connectionId);
        modelo->outputConnectionCreated(connectionId);
    }
}

void DataFlowGraphModel::sendConnectionDeletion(ConnectionId const connectionId)
{
    Q_EMIT connectionDeleted(connectionId);

    auto iti = _models.find(connectionId.inNodeId);
    auto ito = _models.find(connectionId.outNodeId);
    if (iti != _models.end() && ito != _models.end()) {
        auto &modeli = iti->second.nodeModel;
        auto &modelo = ito->second.nodeModel;
        modeli->inputConnectionDeleted(connectionId);
        modelo->outputConnectionDeleted(connectionId);
    }
}

bool DataFlowGraphModel::nodeExists(NodeId const nodeId) const
{
    return (_models.find(nodeId) != _models.end());
}

QVariant DataFlowGraphModel::nodeData(NodeId nodeId, NodeRole role) const
{
    QVariant result;

    auto it = _models.find(nodeId);
    if (it == _models.end())
        return result;

    auto &model = it->second.nodeModel;

    switch (role) {
    case NodeRole::Type:
        result = model->name();
        break;

    case NodeRole::Position:
        result = _nodeGeometryData[nodeId].pos;
        break;

    case NodeRole::Size:
        result = _nodeGeometryData[nodeId].size;
        break;

    case NodeRole::CaptionVisible:
        result = model->captionVisible();
        break;

    case NodeRole::Caption:
        result = model->caption();
        break;

    case NodeRole::Style: {
        auto style = StyleCollection::nodeStyle();
        result = style.toJson().toVariantMap();
    } break;

    case NodeRole::InternalData: {
        QJsonObject nodeJson;

        nodeJson["internal-data"] = _models.at(nodeId).nodeModel->save();

        result = nodeJson.toVariantMap();
        break;
    }

    case NodeRole::InPortCount:
        result = model->nPorts(PortType::In);
        break;

    case NodeRole::OutPortCount:
        result = model->nPorts(PortType::Out);
        break;

    case NodeRole::Widget: {
        auto w = model->embeddedWidget();
        result = QVariant::fromValue(w);
    } break;
    }

    return result;
}

NodeFlags DataFlowGraphModel::nodeFlags(NodeId nodeId) const
{
    auto it = _models.find(nodeId);

    if (it != _models.end() && it->second.nodeModel->resizable())
        return NodeFlag::Resizable;

    return NodeFlag::NoFlags;
}

bool DataFlowGraphModel::setNodeData(NodeId nodeId, NodeRole role, QVariant value)
{
    Q_UNUSED(nodeId);
    Q_UNUSED(role);
    Q_UNUSED(value);

    bool result = false;

    switch (role) {
    case NodeRole::Type:
        break;
    case NodeRole::Position: {
        _nodeGeometryData[nodeId].pos = value.value<QPointF>();

        Q_EMIT nodePositionUpdated(nodeId);

        result = true;
    } break;

    case NodeRole::Size: {
        _nodeGeometryData[nodeId].size = value.value<QSize>();
        result = true;
    } break;

    case NodeRole::CaptionVisible:
        break;

    case NodeRole::Caption:
        break;

    case NodeRole::Style:
        break;

    case NodeRole::InternalData:
        break;

    case NodeRole::InPortCount:
        break;

    case NodeRole::OutPortCount:
        break;

    case NodeRole::Widget:
        break;
    }

    return result;
}

QVariant DataFlowGraphModel::portData(NodeId nodeId,
                                      PortType portType,
                                      PortIndex portIndex,
                                      PortRole role) const
{
    QVariant result;

    auto it = _models.find(nodeId);
    if (it == _models.end())
        return result;

    auto &model = it->second.nodeModel;

    switch (role) {
    case PortRole::Data:
        if (portType == PortType::Out)
            result = QVariant::fromValue(model->outData(portIndex));
        break;

    case PortRole::DataType:
        result = QVariant::fromValue(model->dataType(portType, portIndex));
        break;

    case PortRole::ConnectionPolicyRole:
        result = QVariant::fromValue(model->portConnectionPolicy(portType, portIndex));
        break;

    case PortRole::CaptionVisible:
        result = model->portCaptionVisible(portType, portIndex);
        break;

    case PortRole::Caption:
        result = model->portCaption(portType, portIndex);

        break;
    }

    return result;
}

bool DataFlowGraphModel::setPortData(
    NodeId nodeId, PortType portType, PortIndex portIndex, QVariant const &value, PortRole role)
{
    Q_UNUSED(nodeId);

    QVariant result;

    auto it = _models.find(nodeId);
    if (it == _models.end())
        return false;

    auto &model = it->second.nodeModel;

    switch (role) {
    case PortRole::Data:
        if (portType == PortType::In) {
            model->setInData(value.value<std::shared_ptr<NodeData>>(), portIndex);

            // Triggers repainting on the scene.
            Q_EMIT inPortDataWasSet(nodeId, portType, portIndex);
        }
        break;

    default:
        break;
    }

    return false;
}

bool DataFlowGraphModel::deleteConnection(ConnectionId const connectionId)
{
    bool disconnected = false;

    auto it = _connectivity.find(connectionId);

    if (it != _connectivity.end()) {
        disconnected = true;

        //count == 0 Example Remove a successor node from the topology
        int count = --_models.at(connectionId.outNodeId).successorNodes[connectionId.inNodeId];
        if(count == 0)
        {
            _models.at(connectionId.outNodeId).successorNodes.erase(connectionId.inNodeId);
        }

        _connectivity.erase(it);
    }


    if (disconnected) {
        sendConnectionDeletion(connectionId);

        propagateEmptyDataTo(getNodeId(PortType::In, connectionId),
                             getPortIndex(PortType::In, connectionId));
    }

    return disconnected;
}

bool DataFlowGraphModel::deleteNode(NodeId const nodeId)
{
    // Delete connections to this node first.
    auto connectionIds = allConnectionIds(nodeId);
    for (auto &cId : connectionIds) {
        deleteConnection(cId);
    }

    _nodeGeometryData.erase(nodeId);
    _models.erase(nodeId);

    Q_EMIT nodeDeleted(nodeId);

    return true;
}

QJsonObject DataFlowGraphModel::saveNode(NodeId const nodeId) const
{
    QJsonObject nodeJson;

    nodeJson["id"] = static_cast<qint64>(nodeId);

    nodeJson["internal-data"] = _models.at(nodeId).nodeModel->save();

    {
        QPointF const pos = nodeData(nodeId, NodeRole::Position).value<QPointF>();

        QJsonObject posJson;
        posJson["x"] = pos.x();
        posJson["y"] = pos.y();
        nodeJson["position"] = posJson;
    }

    return nodeJson;
}

QJsonObject DataFlowGraphModel::save() const
{
    QJsonObject sceneJson;

    QJsonArray nodesJsonArray;
    for (auto const nodeId : allNodeIds()) {
        nodesJsonArray.append(saveNode(nodeId));
    }
    sceneJson["nodes"] = nodesJsonArray;

    QJsonArray connJsonArray;
    for (auto const &cid : _connectivity) {
        connJsonArray.append(toJson(cid));
    }
    sceneJson["connections"] = connJsonArray;

    return sceneJson;
}

void DataFlowGraphModel::loadNode(QJsonObject const &nodeJson)
{
    // Possibility of the id clash when reading it from json and not generating a
    // new value.
    // 1. When restoring a scene from a file.
    // Conflict is not possible because the scene must be cleared by the time of
    // loading.
    // 2. When undoing the deletion command.  Conflict is not possible
    // because all the new ids were created past the removed nodes.
    NodeId restoredNodeId = nodeJson["id"].toInt();

    _nextNodeId = std::max(_nextNodeId, restoredNodeId + 1);

    QJsonObject const internalDataJson = nodeJson["internal-data"].toObject();

    QString delegateModelName = internalDataJson["model-name"].toString();

    std::unique_ptr<NodeDelegateModel> model = _registry->create(delegateModelName);

    if (model) {
        model->initialize();

        connect(model.get(),
                &NodeDelegateModel::dataUpdated,
                [restoredNodeId, this](PortIndex const portIndex) {
                    onOutPortDataUpdated(restoredNodeId, portIndex);
                });

        _models[restoredNodeId].nodeModel = std::move(model);

        Q_EMIT nodeCreated(restoredNodeId);

        QJsonObject posJson = nodeJson["position"].toObject();
        QPointF const pos(posJson["x"].toDouble(), posJson["y"].toDouble());

        setNodeData(restoredNodeId, NodeRole::Position, pos);

        _models[restoredNodeId].nodeModel->load(internalDataJson);
    } else {
        throw std::logic_error(std::string("No registered model with name ")
                               + delegateModelName.toLocal8Bit().data());
    }
}

void DataFlowGraphModel::load(QJsonObject const &jsonDocument)
{
    QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

    for (QJsonValueRef nodeJson : nodesJsonArray) {
        loadNode(nodeJson.toObject());
    }

    QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();

    for (QJsonValueRef connection : connectionJsonArray) {
        QJsonObject connJson = connection.toObject();

        ConnectionId connId = fromJson(connJson);

        // Restore the connection
        addConnection(connId);
    }
}

void DataFlowGraphModel::onOutPortDataUpdated(NodeId const nodeId, PortIndex const portIndex)
{
    std::unordered_set<ConnectionId> const &connected = connections(nodeId,
                                                                    PortType::Out,
                                                                    portIndex);

    QVariant const portDataToPropagate = portData(nodeId, PortType::Out, portIndex, PortRole::Data);

    for (auto const &cn : connected) {
        setPortData(cn.inNodeId, PortType::In, cn.inPortIndex, portDataToPropagate, PortRole::Data);
    }
}

void DataFlowGraphModel::propagateEmptyDataTo(NodeId const nodeId, PortIndex const portIndex)
{
    QVariant emptyData{};

    setPortData(nodeId, PortType::In, portIndex, emptyData, PortRole::Data);
}

} // namespace QtNodes
