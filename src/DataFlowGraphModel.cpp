#include "DataFlowGraphModel.hpp"
#include "ConnectionIdHash.hpp"

#include <QJsonArray>

namespace QtNodes
{


DataFlowGraphModel::
DataFlowGraphModel(std::shared_ptr<NodeDelegateModelRegistry> registry)
  : _registry(registry)
  , _nextNodeId{0}
{}


std::unordered_set<NodeId>
DataFlowGraphModel::
allNodeIds() const
{
  std::unordered_set<NodeId> nodeIds;
  for_each(_models.begin(), _models.end(),
           [&nodeIds](auto const& p)
           {
             nodeIds.insert(p.first);

           });

  return nodeIds;
}


std::unordered_set<ConnectionId>
DataFlowGraphModel::
allConnectionIds(NodeId const nodeId) const
{
  std::unordered_set<ConnectionId> result;

  for (auto& c : _connectivity)
  {
    if (std::get<0>(c.first) != nodeId)
      continue;

    PortType const portType = std::get<1>(c.first);
    PortIndex const portIndex = std::get<2>(c.first);

    for (auto& target : c.second)
    {
      ConnectionId conn{nodeId,
                        portIndex,
                        target.first,
                        target.second};

      if (portType == PortType::In)
      {
        invertConnection(conn);
      }
      result.insert(conn);
    }
  }

  return result;
}


std::unordered_set<ConnectionId>
DataFlowGraphModel::
connections(NodeId    nodeId,
            PortType  portType,
            PortIndex portIndex) const
{
  std::unordered_set<ConnectionId> result;

  auto const key = std::make_tuple(nodeId, portType, portIndex);

  auto it = _connectivity.find(key);

  if (it != _connectivity.end())
  {
    for (auto& nodeAndPort : it->second)
    {
      ConnectionId conn{nodeId,
                        portIndex,
                        nodeAndPort.first,
                        nodeAndPort.second};

      if (portType == PortType::In)
      {
        invertConnection(conn);
      }
      result.insert(conn);
    }
  }

  return result;
}


bool
DataFlowGraphModel::
connectionExists(ConnectionId const connectionId) const
{
  auto key =
    std::make_tuple(getNodeId(PortType::Out, connectionId),
                    PortType::Out,
                    getPortIndex(PortType::Out, connectionId));

  return (_connectivity.find(key) != _connectivity.end());
}


NodeId
DataFlowGraphModel::
addNode(QString const nodeType)
{
  std::unique_ptr<NodeDelegateModel> model = _registry->create(nodeType);

  if (model)
  {
    NodeId newId = newNodeId();

    connect(model.get(), &NodeDelegateModel::dataUpdated,
            [newId, this](PortIndex const portIndex)
            { onNodeDataUpdated(newId, portIndex); });

    _models[newId] = std::move(model);

    Q_EMIT nodeCreated(newId);

    return newId;
  }

  return InvalidNodeId;
}


bool
DataFlowGraphModel::
connectionPossible(ConnectionId const connectionId) const
{
  auto getDataType =
    [&](PortType const portType)
    {
      return portData(getNodeId(portType, connectionId),
                      portType,
                      getPortIndex(portType, connectionId),
                      PortRole::DataType).value<NodeDataType>();

    };

  auto portVacant =
    [&](PortType const portType)
    {
      NodeId const nodeId = getNodeId(portType, connectionId);
      PortIndex const portIndex = getPortIndex(portType, connectionId);
      auto const connected = connections(nodeId, portType, portIndex);

      auto policy = portData(nodeId,
                             portType,
                             portIndex,
                             PortRole::ConnectionPolicyRole).value<ConnectionPolicy>();

      return connected.empty() || (policy == ConnectionPolicy::Many);
    };

  return getDataType(PortType::Out).id == getDataType(PortType::In).id &&
         portVacant(PortType::Out) && portVacant(PortType::In);
}


void
DataFlowGraphModel::
addConnection(ConnectionId const connectionId)
{
  auto connect =
    [&](PortType portType)
    {
      auto key = ConnectivityKey{getNodeId(portType, connectionId),
                                 portType,
                                 getPortIndex(portType, connectionId)};

      PortType opposite = oppositePort(portType);

      _connectivity[key].insert(std::make_pair(getNodeId(opposite, connectionId),
                                               getPortIndex(opposite, connectionId)));
    };

  connect(PortType::Out);
  connect(PortType::In);

  Q_EMIT connectionCreated(connectionId);

  onNodeDataUpdated(getNodeId(PortType::Out, connectionId),
                    getPortIndex(PortType::Out, connectionId));
}


bool
DataFlowGraphModel::
nodeExists(NodeId const nodeId) const
{
  return (_models.find(nodeId) != _models.end());
}


QVariant
DataFlowGraphModel::
nodeData(NodeId nodeId, NodeRole role) const
{
  QVariant result;

  auto it = _models.find(nodeId);
  if (it == _models.end())
    return result;

  auto& model = it->second;

  switch (role)
  {
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

    case NodeRole::Style:
    {
      auto style = StyleCollection::nodeStyle();
      result = style.toJson().toVariantMap();
    }
    break;

    case NodeRole::InternalData:
    {
      QJsonObject nodeJson;

      nodeJson["internal-data"] = _models.at(nodeId)->save();

      result = nodeJson.toVariantMap();
      break;
    }

    case NodeRole::NumberOfInPorts:
      result = model->nPorts(PortType::In);
      break;

    case NodeRole::NumberOfOutPorts:
      result = model->nPorts(PortType::Out);
      break;

    case NodeRole::Widget:
    {
      auto w = model->embeddedWidget();
      result = QVariant::fromValue(w);
    }
    break;
  }

  return result;
}


NodeFlags
DataFlowGraphModel::
nodeFlags(NodeId nodeId) const
{
  auto it = _models.find(nodeId);

  if (it != _models.end() && it->second->resizable())
    return NodeFlag::Resizable;

  return NodeFlag::NoFlags;
}


bool
DataFlowGraphModel::
setNodeData(NodeId   nodeId,
            NodeRole role,
            QVariant value)
{
  Q_UNUSED(nodeId);
  Q_UNUSED(role);
  Q_UNUSED(value);

  bool result = false;

  switch (role)
  {
    case NodeRole::Type:
      break;
    case NodeRole::Position:
    {
      _nodeGeometryData[nodeId].pos = value.value<QPointF>();

      Q_EMIT nodePositionUpdated(nodeId);

      result = true;
    }
    break;

    case NodeRole::Size:
    {
      _nodeGeometryData[nodeId].size = value.value<QSize>();
      result = true;
    }
    break;

    case NodeRole::CaptionVisible:
      break;

    case NodeRole::Caption:
      break;

    case NodeRole::Style:
      break;

    case NodeRole::InternalData:
      break;

    case NodeRole::NumberOfInPorts:
      break;

    case NodeRole::NumberOfOutPorts:
      break;

    case NodeRole::Widget:
      break;
  }

  return result;
}


QVariant
DataFlowGraphModel::
portData(NodeId    nodeId,
         PortType  portType,
         PortIndex portIndex,
         PortRole  role) const
{
  QVariant result;

  auto it = _models.find(nodeId);
  if (it == _models.end())
    return result;

  auto& model = it->second;

  switch (role)
  {
    case PortRole::Data:
      if (portType == PortType::Out)
        result = QVariant::fromValue(model->outData(portIndex));
      break;

    case PortRole::DataType:
      result = QVariant::fromValue(model->dataType(portType, portIndex));
      break;

    case PortRole::ConnectionPolicyRole:
      result =
        QVariant::fromValue(model->portConnectionPolicy(portType,
                                                        portIndex));
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


bool
DataFlowGraphModel::
setPortData(NodeId    nodeId,
            PortType  portType,
            PortIndex portIndex,
            PortRole  role) const
{
  Q_UNUSED(nodeId);
  Q_UNUSED(portType);
  Q_UNUSED(portIndex);
  Q_UNUSED(role);

  return false;
}


bool
DataFlowGraphModel::
deleteConnection(ConnectionId const connectionId)
{
  bool disconnected = false;

  auto disconnect =
    [&](PortType portType)
    {
      auto key = ConnectivityKey{getNodeId(portType, connectionId),
                                 portType,
                                 getPortIndex(portType, connectionId)};
      auto it = _connectivity.find(key);

      if (it != _connectivity.end())
      {
        disconnected = true;

        PortType opposite = oppositePort(portType);

        auto oppositePair =
          std::make_pair(getNodeId(opposite, connectionId),
                         getPortIndex(opposite, connectionId));
        it->second.erase(oppositePair);

        if (it->second.empty())
        {
          _connectivity.erase(it);
        }
      }

    };

  disconnect(PortType::Out);
  disconnect(PortType::In);

  if (disconnected)
  {
    Q_EMIT connectionDeleted(connectionId);

    propagateEmptyDataTo(getNodeId(PortType::In, connectionId),
                         getPortIndex(PortType::In, connectionId));

  }

  return disconnected;
}


bool
DataFlowGraphModel::
deleteNode(NodeId const nodeId)
{
  // Delete connections to this node first.
  auto connectionIds = allConnectionIds(nodeId);
  for (auto& cId : connectionIds)
  {
    deleteConnection(cId);
  }

  _nodeGeometryData.erase(nodeId);
  _models.erase(nodeId);

  Q_EMIT nodeDeleted(nodeId);

  return true;
}


QJsonObject
DataFlowGraphModel::
saveNode(NodeId const nodeId) const
{
  QJsonObject nodeJson;

  nodeJson["id"] = static_cast<qint64>(nodeId);

  nodeJson["internal-data"] = _models.at(nodeId)->save();

  {
    QPointF const pos =
      nodeData(nodeId, NodeRole::Position).value<QPointF>();

    QJsonObject posJson;
    posJson["x"] = pos.x();
    posJson["y"] = pos.y();
    nodeJson["position"] = posJson;
  }

  return nodeJson;
}


QJsonDocument
DataFlowGraphModel::
save() const
{
  QJsonObject sceneJson;

  QJsonArray nodesJsonArray;
  for (auto const nodeId : allNodeIds())
  {
    nodesJsonArray.append(saveNode(nodeId));
  }
  sceneJson["nodes"] = nodesJsonArray;


  QJsonArray connJsonArray;
  for (auto const & connPair : _connectivity)
  {
    ConnectivityKey const & key = connPair.first;
    auto const& otherSideConnections = connPair.second;

    for (auto const & otherSide : otherSideConnections)
    {
      if (std::get<1>(key) == PortType::Out)
      {
        connJsonArray.append(
          saveConnection(ConnectionId{std::get<0>(key),
                                      std::get<2>(key),
                                      otherSide.first,
                                      otherSide.second}));
      }
    }
  }
  sceneJson["connections"] = connJsonArray;

  return QJsonDocument(sceneJson);
}


void
DataFlowGraphModel::
loadNode(QJsonObject const & nodeJson)
{
  NodeId restoredNodeId = static_cast<NodeId>(nodeJson["id"].toInt());

  // Insert the desired next node id
  setNextNodeId(restoredNodeId);

  QJsonObject const internalDataJson = nodeJson["internal-data"].toObject();

  QString delegateModelName = internalDataJson["model-name"].toString();

  std::unique_ptr<NodeDelegateModel> model = _registry->create(delegateModelName);

  if (model)
  {
    connect(model.get(), &NodeDelegateModel::dataUpdated,
            [restoredNodeId, this](PortIndex const portIndex)
            { onNodeDataUpdated(restoredNodeId, portIndex); });

    _models[restoredNodeId] = std::move(model);

    Q_EMIT nodeCreated(restoredNodeId);

    //

    QJsonObject posJson = nodeJson["position"].toObject();
    QPointF const pos(posJson["x"].toDouble(),
                      posJson["y"].toDouble());

    setNodeData(restoredNodeId,
                NodeRole::Position,
                pos);


    _models[restoredNodeId]->load(internalDataJson);
  }
}


void
DataFlowGraphModel::
load(QJsonDocument const &json)
{
  QJsonObject const jsonDocument = json.object();

  QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

  for (QJsonValueRef node : nodesJsonArray)
  {
    loadNode(node.toObject());
  }

  QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();

  for (QJsonValueRef connection : connectionJsonArray)
  {
    loadConnection(connection.toObject());
  }
}



QJsonObject
DataFlowGraphModel::
saveConnection(ConnectionId const & connId) const
{
  QJsonObject connJson;

  connJson["outNodeId"] = static_cast<qint64>(connId.outNodeId);
  connJson["outPortIndex"] = static_cast<qint64>(connId.outPortIndex);
  connJson["intNodeId"] = static_cast<qint64>(connId.inNodeId);
  connJson["inPortIndex"] = static_cast<qint64>(connId.inPortIndex);

  return connJson;
}


void
DataFlowGraphModel::
loadConnection(QJsonObject const & connJson)
{
  ConnectionId connId{static_cast<NodeId>(connJson["outNodeId"].toInt()),
                      static_cast<PortIndex>(connJson["outPortIndex"].toInt()),
                      static_cast<NodeId>(connJson["intNodeId"].toInt()),
                      static_cast<PortIndex>(connJson["inPortIndex"].toInt())};

  addConnection(connId);
}


void
DataFlowGraphModel::
onNodeDataUpdated(NodeId const    nodeId,
                  PortIndex const portIndex)
{
  std::unordered_set<ConnectionId> const& connected =
    connections(nodeId, PortType::Out, portIndex);

  // We coudl also pull the data through the model::portData
  auto const outPortData = _models[nodeId]->outData(portIndex);

  for (auto const& cn : connected)
  {
    _models[cn.inNodeId]->setInData(outPortData, cn.inPortIndex);

    Q_EMIT portDataSet(cn.inNodeId, PortType::In, cn.inPortIndex);
  }
}


void

DataFlowGraphModel::
propagateEmptyDataTo(NodeId const    nodeId,
                     PortIndex const portIndex)
{
  auto const emptyData = std::shared_ptr<NodeData>();

  _models[nodeId]->setInData(emptyData, portIndex);

  Q_EMIT portDataSet(nodeId, PortType::In, portIndex);
}


}

