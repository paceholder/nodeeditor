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
        NodeId newId = newNodeId();

        initModelFormId(std::move(model), newId);

        _models[newId]->node->init();

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

    return getDataType(PortType::Out) == getDataType(PortType::In) && portVacant(PortType::Out)
           && portVacant(PortType::In);
}

void DataFlowGraphModel::addConnection(ConnectionId const connectionId)
{
    _connectivity.insert(connectionId);

    Q_EMIT connectionCreated(connectionId);

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

    auto &model = it->second;

    switch (role) {
    case NodeRole::Type:
        result = model->node->name();
        break;

    case NodeRole::Position:
        result = _nodeGeometryData[nodeId].pos;
        break;

    case NodeRole::Size:
        result = _nodeGeometryData[nodeId].size;
        break;

    case NodeRole::Caption:
        result = model->node->caption();
        break;

    case NodeRole::Style: {
        auto style = StyleCollection::nodeStyle();
        result = style.toJson().toVariantMap();
    } break;

    case NodeRole::InternalData: {
        QJsonObject nodeJson;

        nodeJson["internal-data"] = _models.at(nodeId)->node->save();

        result = nodeJson.toVariantMap();
        break;
    }

    case NodeRole::InPortCount:
        result = model->ports->nPorts(PortType::In);
        break;

    case NodeRole::OutPortCount:
        result = model->ports->nPorts(PortType::Out);
        break;

    case NodeRole::Widget: {
        auto w = model->node->embeddedWidget();
        result = QVariant::fromValue(w);
    } break;
    }

    return result;
}

NodeFlags DataFlowGraphModel::nodeFlags(NodeId nodeId) const
{
    auto it = _models.find(nodeId);

    if (it != _models.end() && it->second->node->resizable())
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

    auto &model = it->second;

    switch (role) {
    case PortRole::Data:
        if (portType == PortType::Out)
            result = model->ports->portData(PortType::Out, portIndex)->data;
        break;

    case PortRole::DataType:
        result = QVariant::fromValue(model->ports->portData(portType, portIndex)->type());
        break;

    case PortRole::ConnectionPolicyRole:
        result = QVariant::fromValue(model->ports->portConnectionPolicy(portType, portIndex));
        break;

    case PortRole::Caption:
        result = model->ports->portCaption(portType, portIndex);

        break;
    }

    return result;
}

bool DataFlowGraphModel::setPortData(
    NodeId nodeId, PortType portType, PortIndex portIndex, QVariant const &value, PortRole role)
{
    auto it = _models.find(nodeId);
    if (it == _models.end())
        return false;

    auto &model = it->second;

    switch (role) {
    case PortRole::Data:
        if (portType == PortType::In) {
            model->node->setInData(value, portIndex);

            auto d = model->ports->portData(PortType::In, portIndex);
            d->data = value;

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

        _connectivity.erase(it);
    }

    if (disconnected) {
        Q_EMIT connectionDeleted(connectionId);

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

    nodeJson["model-name"] = _models.at(nodeId)->node->name();

    nodeJson["internal-data"] = _models.at(nodeId)->node->save();

    if (_models.at(nodeId)->node->isDynamicPorts()) {
        QJsonArray inPortsArray;
        for (unsigned int i = 0; i < _models.at(nodeId)->ports->nPorts(PortType::In); i++) {
            auto p = _models.at(nodeId)->ports->port(PortType::In, i);
            QJsonObject portJson;
            portJson["dataType"] = p.data->type();
            portJson["name"] = p.name;
            portJson["policy"] = (int) p.connectionPolicy;

            inPortsArray.append(portJson);
        }
        nodeJson["inputPorts"] = inPortsArray;

        QJsonArray outPortsArray;
        for (unsigned int i = 0; i < _models.at(nodeId)->ports->nPorts(PortType::Out); i++) {
            auto p = _models.at(nodeId)->ports->port(PortType::Out, i);
            QJsonObject portJson;
            portJson["dataType"] = p.data->type();
            portJson["name"] = p.name;
            portJson["policy"] = (int) p.connectionPolicy;

            outPortsArray.append(portJson);
        }
        nodeJson["outputPorts"] = outPortsArray;
    }

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

    QString delegateModelName = nodeJson["model-name"].toString();

    QJsonObject const internalDataJson = nodeJson["internal-data"].toObject();

    std::unique_ptr<NodeDelegateModel> model = _registry->create(delegateModelName);

    if (model) {
        initModelFormId(std::move(model), restoredNodeId);

        if (_models.at(restoredNodeId)->node->isDynamicPorts()) {
            QJsonArray inputPortsArray = nodeJson["inputPorts"].toArray();
            QJsonArray outputPortsArray = nodeJson["outputPorts"].toArray();

            for (auto port : inputPortsArray) {
                std::shared_ptr<NodeData> d = _registry->createData(
                    port.toObject().value("dataType").toString());

                if (d)
                    _models[restoredNodeId]->ports->createPort(
                        PortType::In,
                        std::move(d),
                        port.toObject().value("name").toString(),
                        (QtNodes::ConnectionPolicy) port.toObject().value("policy").toInt());
            }

            for (auto port : outputPortsArray) {
                std::shared_ptr<NodeData> d = _registry->createData(
                    port.toObject().value("dataType").toString());

                if (d)
                    _models[restoredNodeId]->ports->createPort(
                        PortType::Out,
                        std::move(d),
                        port.toObject().value("name").toString(),
                        (QtNodes::ConnectionPolicy) port.toObject().value("policy").toInt());
            }

        } else {
            _models[restoredNodeId]->node->init();
        }

        Q_EMIT nodeCreated(restoredNodeId);

        QJsonObject posJson = nodeJson["position"].toObject();
        QPointF const pos(posJson["x"].toDouble(), posJson["y"].toDouble());

        setNodeData(restoredNodeId, NodeRole::Position, pos);

        _models[restoredNodeId]->node->load(internalDataJson);
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

void DataFlowGraphModel::initModelFormId(std::unique_ptr<NodeDelegateModel> model,
                                         NodeId const nodeId)
{
    connect(model.get(),
            &NodeDelegateModel::dataUpdated,
            [nodeId, this](PortIndex const portIndex) { onOutPortDataUpdated(nodeId, portIndex); });

    connect(model.get(),
            &NodeDelegateModel::createPort,
            this,
            [nodeId, this](PortType portType,
                           const NodeDataType dataType,
                           const PortCaption name,
                           ConnectionPolicy policy) {
                std::shared_ptr<NodeData> d = _registry->createData(dataType);

                if (d) {
                    _models[nodeId]->ports->createPort(portType, std::move(d), name, policy);

                    nodeUpdated(nodeId);
                }
            });

    connect(model.get(),
            &NodeDelegateModel::insertPort,
            this,
            [nodeId, this](PortType portType,
                           PortIndex portIndex,
                           const NodeDataType dataType,
                           const PortCaption name,
                           ConnectionPolicy policy) {
                std::shared_ptr<NodeData> d = _registry->createData(dataType);

                if (d) {
                    _models[nodeId]->ports->insertPort(portType,
                                                       portIndex,
                                                       std::move(d),
                                                       name,
                                                       policy);

                    portsAboutToBeInserted(nodeId, portType, portIndex, portIndex);

                    portsInserted();

                    nodeUpdated(nodeId);
                }
            });

    connect(model.get(),
            &NodeDelegateModel::removePort,
            this,
            [nodeId, this](PortType portType, PortIndex portIndex) {
                _models[nodeId]->ports->removePort(portType, portIndex);

                portsAboutToBeDeleted(nodeId, portType, portIndex, portIndex);

                portsDeleted();

                nodeUpdated(nodeId);
            });

    connect(model.get(),
            &NodeDelegateModel::updateOutPortData,
            this,
            [nodeId, this](PortIndex portIndex, QVariant nodeData) {
                std::shared_ptr<NodeData> d = _models[nodeId]->ports->portData(PortType::Out,
                                                                               portIndex);
                d->data = nodeData;
            });

    connect(model.get(),
            &NodeDelegateModel::updatePortCaption,
            this,
            [nodeId, this](PortType portType, PortIndex portIndex, const PortCaption name) {
                _models[nodeId]->ports->setPortCaption(portType, portIndex, name);
            });

    connect(model.get(),
            &NodeDelegateModel::updatePortConnectionPolicy,
            this,
            [nodeId, this](PortType portType, PortIndex portIndex, ConnectionPolicy policy) {
                _models[nodeId]->ports->setPortConnectionPolicy(portType, portIndex, policy);
            });

    _models[nodeId] = std::make_unique<NodeModel>();
    _models[nodeId]->node = std::move(model);
    _models[nodeId]->ports = std::make_unique<NodePorts>();
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
