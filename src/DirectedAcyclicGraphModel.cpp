#include "DirectedAcyclicGraphModel.hpp"
#include "ConnectionIdHash.hpp"

#include <QJsonArray>

#include <stack>
#include <stdexcept>

namespace {
bool isCyclicUtil(QtNodes::NodeId nodeId,
                  std::unordered_map<QtNodes::NodeId, bool> &visited,
                  std::unordered_map<QtNodes::NodeId, bool> &recStack,
                  std::unordered_set<QtNodes::ConnectionId> const &connections)
{
    if (!visited[nodeId]) {
        visited[nodeId] = true;
        recStack[nodeId] = true;

        for (const auto &conn : connections) {
            if (conn.outNodeId == nodeId) {
                QtNodes::NodeId adjacent = conn.inNodeId;
                if (!visited[adjacent] && isCyclicUtil(adjacent, visited, recStack, connections)) {
                    return true;
                }
                if (recStack[adjacent]) {
                    return true;
                }
            }
        }
    }
    recStack[nodeId] = false;
    return false;
}

void topologicalSortUtil(QtNodes::NodeId nodeId,
                         std::unordered_map<QtNodes::NodeId, bool> &visited,
                         std::stack<QtNodes::NodeId> &stack,
                         std::unordered_set<QtNodes::ConnectionId> const &connections)
{
    visited[nodeId] = true;

    for (const auto &conn : connections) {
        if (conn.outNodeId == nodeId) {
            QtNodes::NodeId adjacent = conn.inNodeId;
            if (!visited[adjacent]) {
                topologicalSortUtil(adjacent, visited, stack, connections);
            }
        }
    }
    stack.push(nodeId);
}

void isConnectedUtil(QtNodes::NodeId nodeId,
                     std::unordered_map<QtNodes::NodeId, bool> &visited,
                     std::stack<QtNodes::NodeId> &stack,
                     std::unordered_set<QtNodes::ConnectionId> const &connections)
{
    visited[nodeId] = true;

    for (const auto &conn : connections) {
        // we don't care about direction, just connectivity
        if (conn.inNodeId == nodeId || conn.outNodeId == nodeId) {
            QtNodes::NodeId adjacent = conn.inNodeId == nodeId ? conn.outNodeId : conn.inNodeId;
            if (!visited[adjacent]) {
                isConnectedUtil(adjacent, visited, stack, connections);
            }
        }
    }
    stack.push(nodeId);
}
} // namespace

namespace QtNodes {

DirectedAcyclicGraphModel::DirectedAcyclicGraphModel(
    std::shared_ptr<NodeDelegateModelRegistry> registry)
    : _registry(std::move(registry))
    , _nextNodeId{0}
{
    connect(this,
            &DirectedAcyclicGraphModel::nodeCreated,
            this,
            &DirectedAcyclicGraphModel::onNodeCreated);
}

std::unordered_set<NodeId> DirectedAcyclicGraphModel::allNodeIds() const
{
    std::unordered_set<NodeId> nodeIds;
    for_each(_models.begin(), _models.end(), [&nodeIds](auto const &p) { nodeIds.insert(p.first); });

    return nodeIds;
}

std::unordered_set<ConnectionId> DirectedAcyclicGraphModel::allConnectionIds(NodeId const nodeId) const
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

std::unordered_set<ConnectionId> DirectedAcyclicGraphModel::connections(NodeId nodeId,
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

bool DirectedAcyclicGraphModel::connectionExists(ConnectionId const connectionId) const
{
    return (_connectivity.find(connectionId) != _connectivity.end());
}

NodeId DirectedAcyclicGraphModel::addNode(QString const nodeType)
{
    std::unique_ptr<NodeDelegateModel> model = _registry->create(nodeType);

    if (model) {
        NodeId newId = newNodeId();

        _models[newId] = std::move(model);

        Q_EMIT nodeCreated(newId);

        _isCyclicCache.clear();

        return newId;
    }

    return InvalidNodeId;
}

bool DirectedAcyclicGraphModel::connectionPossible(ConnectionId const connectionId) const
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
           && portVacant(PortType::Out) && portVacant(PortType::In) && !willBeCyclic(connectionId);
}

void DirectedAcyclicGraphModel::addConnection(ConnectionId const connectionId)
{
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

    // sanity check
    isCyclic();
}

void DirectedAcyclicGraphModel::sendConnectionCreation(ConnectionId const connectionId)
{
    Q_EMIT connectionCreated(connectionId);

    auto iti = _models.find(connectionId.inNodeId);
    auto ito = _models.find(connectionId.outNodeId);
    if (iti != _models.end() && ito != _models.end()) {
        auto &modeli = iti->second;
        auto &modelo = ito->second;
        modeli->inputConnectionCreated(connectionId);
        modelo->outputConnectionCreated(connectionId);
    }
}

void DirectedAcyclicGraphModel::sendConnectionDeletion(ConnectionId const connectionId)
{
    Q_EMIT connectionDeleted(connectionId);

    auto iti = _models.find(connectionId.inNodeId);
    auto ito = _models.find(connectionId.outNodeId);
    if (iti != _models.end() && ito != _models.end()) {
        auto &modeli = iti->second;
        auto &modelo = ito->second;
        modeli->inputConnectionDeleted(connectionId);
        modelo->outputConnectionDeleted(connectionId);
    }
}

bool DirectedAcyclicGraphModel::isCyclic(
    std::optional<std::reference_wrapper<const std::unordered_set<ConnectionId>>> connections) const
{
    const std::unordered_set<ConnectionId> &conns = connections ? connections->get()
                                                                : _connectivity;

    // check against cache whether the nodes + connections are already computed
    const auto hash = hashNodesAndConnections(allNodeIds(), conns);
    if (_isCyclicCache.count(hash) > 0)
        return _isCyclicCache.at(hash);

    std::unordered_map<NodeId, bool> visited;
    std::unordered_map<NodeId, bool> recStack;

    for (const auto &node : _models) {
        if (isCyclicUtil(node.first, visited, recStack, conns)) {
            if (!connections) // don't print if we are using custom connections
                // this should never be true, if it is, there is a bug
                qCritical() << "Directed Acyclic Graph model is cyclic";
            _isCyclicCache[hash] = true;
            return true;
        }
    }
    _isCyclicCache[hash] = false;
    return false;
}

bool DirectedAcyclicGraphModel::willBeCyclic(ConnectionId const connectionId) const
{
    std::unordered_set<ConnectionId> copy(_connectivity);
    copy.insert(connectionId);
    return isCyclic(copy);
}

size_t DirectedAcyclicGraphModel::hashNodesAndConnections(
    std::unordered_set<NodeId> const &nodes,
    std::unordered_set<ConnectionId> const &connections) const
{
    size_t hash = 0;
    for (const auto &node : nodes)
        hash ^= std::hash<uint>{}(node) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    for (const auto &connection : connections) {
        hash ^= std::hash<ConnectionId>{}(connection) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

void DirectedAcyclicGraphModel::initNodeConnections(const std::unique_ptr<NodeDelegateModel> &node,
                                                    const NodeId &id)
{
    connect(node.get(), &NodeDelegateModel::dataUpdated, [id, this](PortIndex const portIndex) {
        onOutPortDataUpdated(id, portIndex);
    });

    connect(node.get(),
            &NodeDelegateModel::portsAboutToBeDeleted,
            this,
            [id, this](PortType const portType, PortIndex const first, PortIndex const last) {
                portsAboutToBeDeleted(id, portType, first, last);
            });

    connect(node.get(),
            &NodeDelegateModel::portsDeleted,
            this,
            &DirectedAcyclicGraphModel::portsDeleted);

    connect(node.get(),
            &NodeDelegateModel::portsAboutToBeInserted,
            this,
            [id, this](PortType const portType, PortIndex const first, PortIndex const last) {
                portsAboutToBeInserted(id, portType, first, last);
            });

    connect(node.get(),
            &NodeDelegateModel::portsInserted,
            this,
            &DirectedAcyclicGraphModel::portsInserted);

    connect(node.get(), &NodeDelegateModel::contentUpdated, [id, this]() {
        Q_EMIT nodeUpdated(id);
    });
}

bool DirectedAcyclicGraphModel::nodeExists(NodeId const nodeId) const
{
    return (_models.find(nodeId) != _models.end());
}

QVariant DirectedAcyclicGraphModel::nodeData(NodeId nodeId, NodeRole role) const
{
    QVariant result;

    auto it = _models.find(nodeId);
    if (it == _models.end())
        return result;

    auto &model = it->second;

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
        auto style = _models.at(nodeId)->nodeStyle();
        result = style.toJson().toVariantMap();
    } break;

    case NodeRole::InternalData: {
        QJsonObject nodeJson;

        nodeJson["internal-data"] = _models.at(nodeId)->save();

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
        break;
    }
    case NodeRole::Shape:
        result = static_cast<int>(_models.at(nodeId)->shape());
        break;
    }

    return result;
}

NodeFlags DirectedAcyclicGraphModel::nodeFlags(NodeId nodeId) const
{
    auto it = _models.find(nodeId);

    if (it != _models.end() && it->second->resizable())
        return NodeFlag::Resizable;

    return NodeFlag::NoFlags;
}

bool DirectedAcyclicGraphModel::setNodeData(NodeId nodeId, NodeRole role, QVariant value)
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

    case NodeRole::Shape:
        break;
    }

    return result;
}

QVariant DirectedAcyclicGraphModel::portData(NodeId nodeId,
                                             PortType portType,
                                             PortIndex portIndex,
                                             PortRole role) const
{
    QVariant result;

    auto it = _models.find(nodeId);
    if (it == _models.end())
        return result;

    auto &model = it->second;

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

    case PortRole::FontColor:
        // Get the color from the map or use black as default
        QColor color = _portFontColors.value({nodeId, portType, portIndex}, QColor(Qt::black));
        result = QVariant::fromValue(color);

        break;
    }

    return result;
}

bool DirectedAcyclicGraphModel::setPortData(
    NodeId nodeId, PortType portType, PortIndex portIndex, QVariant const &value, PortRole role)
{
    Q_UNUSED(nodeId);

    QVariant result;

    auto it = _models.find(nodeId);
    if (it == _models.end())
        return false;

    auto &model = it->second;

    switch (role) {
    case PortRole::Data:
        if (portType == PortType::In) {
            model->setInData(value.value<std::shared_ptr<NodeData>>(), portIndex);

            // Triggers repainting on the scene.
            Q_EMIT inPortDataWasSet(nodeId, portType, portIndex);
        }
        break;

    case PortRole::FontColor:
        _portFontColors[{nodeId, portType, portIndex}] = value.value<QColor>();
        return true;

    default:
        break;
    }

    return false;
}

bool DirectedAcyclicGraphModel::deleteConnection(ConnectionId const connectionId)
{
    bool disconnected = false;

    auto it = _connectivity.find(connectionId);

    if (it != _connectivity.end()) {
        disconnected = true;

        _connectivity.erase(it);
    }

    if (disconnected) {
        sendConnectionDeletion(connectionId);

        propagateEmptyDataTo(getNodeId(PortType::In, connectionId),
                             getPortIndex(PortType::In, connectionId));
    }

    return disconnected;
}

bool DirectedAcyclicGraphModel::deleteNode(NodeId const nodeId)
{
    // Delete connections to this node first.
    auto connectionIds = allConnectionIds(nodeId);
    for (auto &cId : connectionIds) {
        deleteConnection(cId);
    }

    _nodeGeometryData.erase(nodeId);
    _models.erase(nodeId);

    Q_EMIT nodeDeleted(nodeId);

    _isCyclicCache.clear();

    return true;
}

QJsonObject DirectedAcyclicGraphModel::saveNode(NodeId const nodeId) const
{
    QJsonObject nodeJson;

    nodeJson["id"] = static_cast<qint64>(nodeId);

    nodeJson["internal-data"] = _models.at(nodeId)->save();

    {
        QPointF const pos = nodeData(nodeId, NodeRole::Position).value<QPointF>();

        QJsonObject posJson;
        posJson["x"] = pos.x();
        posJson["y"] = pos.y();
        nodeJson["position"] = posJson;
    }

    return nodeJson;
}

QJsonObject DirectedAcyclicGraphModel::save() const
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

void DirectedAcyclicGraphModel::loadNode(QJsonObject const &nodeJson)
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
        _models[restoredNodeId] = std::move(model);

        QJsonObject posJson = nodeJson["position"].toObject();
        QPointF const pos(posJson["x"].toDouble(), posJson["y"].toDouble());

        setNodeData(restoredNodeId, NodeRole::Position, pos);

        _models[restoredNodeId]->load(internalDataJson);
        Q_EMIT nodeCreated(restoredNodeId);
    } else {
        throw std::logic_error(std::string("No registered model with name ")
                               + delegateModelName.toLocal8Bit().data());
    }
}

void DirectedAcyclicGraphModel::load(QJsonObject const &jsonDocument)
{
    QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

    for (QJsonValueRef nodeJson : nodesJsonArray) {
        loadNode(nodeJson.toObject());
    }

    QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();

    std::deque<ConnectionId> orderedConnections;
    for (QJsonValueRef connection : connectionJsonArray) {
        QJsonObject connJson = connection.toObject();

        ConnectionId connId = fromJson(connJson);

        // TODO: better implementation to add connections in a sequential order
        // current implementation prioritizes input index 0 first since it is
        // the function input for reduce blocks, which will create i/o ports for the block
        if (connId.inPortIndex == 0)
            orderedConnections.push_front(connId);
        else
            orderedConnections.push_back(connId);
    }
    for (auto &conn : orderedConnections)
        addConnection(conn);

    Q_EMIT graphLoadedFromFile(nodesJsonArray);
}

std::vector<NodeId> DirectedAcyclicGraphModel::topologicalOrder() const
{
    std::stack<NodeId> stack;
    std::unordered_map<NodeId, bool> visited;

    for (const auto &node : _models) {
        if (!visited[node.first]) {
            topologicalSortUtil(node.first, visited, stack, _connectivity);
        }
    }

    std::vector<NodeId> sortedNodes;
    while (!stack.empty()) {
        sortedNodes.push_back(stack.top());
        stack.pop();
    }
    return sortedNodes;
}

bool DirectedAcyclicGraphModel::isConnected() const
{
    std::stack<NodeId> stack;
    std::unordered_map<NodeId, bool> visited;
    if (_models.size() < 1)
        return false;
    isConnectedUtil(_models.begin()->first, visited, stack, _connectivity);
    return stack.size() == _models.size();
}

void DirectedAcyclicGraphModel::onOutPortDataUpdated(NodeId const nodeId, PortIndex const portIndex)
{
    std::unordered_set<ConnectionId> const &connected = connections(nodeId,
                                                                    PortType::Out,
                                                                    portIndex);

    QVariant const portDataToPropagate = portData(nodeId, PortType::Out, portIndex, PortRole::Data);

    for (auto const &cn : connected) {
        setPortData(cn.inNodeId, PortType::In, cn.inPortIndex, portDataToPropagate, PortRole::Data);
    }
}

void DirectedAcyclicGraphModel::propagateEmptyDataTo(NodeId const nodeId, PortIndex const portIndex)
{
    QVariant emptyData{};

    setPortData(nodeId, PortType::In, portIndex, emptyData, PortRole::Data);
}

void DirectedAcyclicGraphModel::onNodeCreated(NodeId const nodeId)
{
    auto it = _models.find(nodeId);
    if (it == _models.end())
        return;
    auto &model = it->second;
    initNodeConnections(model, nodeId);
}

} // namespace QtNodes
