#include "CustomGraphModel.hpp"


CustomGraphModel::
CustomGraphModel()
  : _nextNodeId{0}
{}


CustomGraphModel::
~CustomGraphModel()
{
  //
}


std::unordered_set<NodeId>
CustomGraphModel::
allNodeIds() const
{
  return _nodeIds;
}


std::unordered_set<ConnectionId>
CustomGraphModel::
allConnectionIds(NodeId const nodeId) const
{
  std::unordered_set<ConnectionId> result;

  for (auto & c : _connectivity)
  {
    if (std::get<0>(c.first) != nodeId)
      continue;

    PortType const portType = std::get<1>(c.first);
    PortIndex const portIndex = std::get<2>(c.first);

    for (auto & target : c.second)
    {
      if (portType == PortType::Out)
      {
        result.insert(ConnectionId{nodeId, portIndex,
                                   target.first, target.second});
      }
      else
      {
        result.insert(ConnectionId{target.first, target.second,
                                   nodeId, portIndex});
      }
    }
  }

  return result;
}


std::unordered_set<ConnectionId>
CustomGraphModel::
connections(NodeId    nodeId,
            PortType  portType,
            PortIndex portIndex) const
{
  std::unordered_set<ConnectionId> result;

  auto const connectivityKey = std::make_tuple(nodeId, portType, portIndex);

  auto it = _connectivity.find(connectivityKey);

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
CustomGraphModel::
connectionExists(ConnectionId const connectionId) const
{
  auto key =
    std::make_tuple(getNodeId(PortType::Out, connectionId),
                    PortType::Out,
                    getPortIndex(PortType::Out, connectionId));

  return (_connectivity.find(key) != _connectivity.end());
}


NodeId
CustomGraphModel::
addNode(QString const nodeType)
{
  NodeId newId = _nextNodeId++;
  // Create new node.
  _nodeIds.insert(newId);

  Q_EMIT nodeCreated(newId);

  return newId;
}


bool
CustomGraphModel::
connectionPossible(ConnectionId const connectionId) const
{
  auto keyOut = std::make_tuple(getNodeId(PortType::Out, connectionId),
                                PortType::Out,
                                getPortIndex(PortType::Out, connectionId));

  auto keyIn = std::make_tuple(getNodeId(PortType::Out, connectionId),
                               PortType::Out,
                               getPortIndex(PortType::Out, connectionId));

  bool result = (_connectivity.find(keyOut) == _connectivity.end() &&
                 _connectivity.find(keyIn) == _connectivity.end());

  return result;
}


void
CustomGraphModel::
addConnection(ConnectionId const connectionId)
{
  auto connect =
    [&](PortType portType)
    {
      auto key = std::make_tuple(getNodeId(portType, connectionId),
                                 portType,
                                 getPortIndex(portType, connectionId));

      PortType opposite = oppositePort(portType);

      _connectivity[key].insert(std::make_pair(getNodeId(opposite, connectionId),
                                               getPortIndex(opposite, connectionId)));
    };

  connect(PortType::Out);
  connect(PortType::In);

  Q_EMIT connectionCreated(connectionId);
}


bool
CustomGraphModel::
nodeExists(NodeId const nodeId) const
{
  return (_nodeIds.find(nodeId) != _nodeIds.end());
}


QVariant
CustomGraphModel::
nodeData(NodeId nodeId, NodeRole role) const
{
  Q_UNUSED(nodeId);

  QVariant result;

  switch (role)
  {
    case NodeRole::Type:
      result = QString("Default Node Type");
      break;

    case NodeRole::Position:
      result = _nodeGeometryData[nodeId].pos;
      break;

    case NodeRole::Size:
      result = _nodeGeometryData[nodeId].size;
      break;

    case NodeRole::CaptionVisible:
      result = true;
      break;

    case NodeRole::Caption:
      result = QString("Node");
      break;

    case NodeRole::Style:
    {
      auto style = StyleCollection::nodeStyle();
      result = style.toJson().toVariant();
    }
    break;

    case NodeRole::InternalData:
      break;

    case NodeRole::NumberOfInPorts:
      result = 1u;
      break;

    case NodeRole::NumberOfOutPorts:
      result = 1u;
      break;

    case NodeRole::Widget:
      result = QVariant();
      break;
  }

  return result;
}


bool
CustomGraphModel::
setNodeData(NodeId   nodeId,
            NodeRole role,
            QVariant value)
{
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
CustomGraphModel::
portData(NodeId    nodeId,
         PortType  portType,
         PortIndex portIndex,
         PortRole  role) const
{
  switch (role)
  {
    case PortRole::Data:
      return QVariant();
      break;

    case PortRole::DataType:
      return QVariant();
      break;

    case PortRole::ConnectionPolicyRole:
      return QVariant::fromValue(ConnectionPolicy::One);
      break;

    case PortRole::CaptionVisible:
      return true;
      break;

    case PortRole::Caption:
      if (portType == PortType::In)
        return QString::fromUtf8("Port In");
      else
        return QString::fromUtf8("Port Out");

      break;
  }

  return QVariant();
}


bool
CustomGraphModel::
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
CustomGraphModel::
deleteConnection(ConnectionId const connectionId)
{
  bool disconnected = false;

  auto disconnect =
    [&](PortType portType)
    {
      auto key = std::make_tuple(getNodeId(portType, connectionId),
                                 portType,
                                 getPortIndex(portType, connectionId));
      auto it = _connectivity.find(key);

      if (it != _connectivity.end())
      {
        disconnected = true;

        PortType opposite = oppositePort(portType);

        auto oppositePair = std::make_pair(getNodeId(opposite, connectionId),
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
    Q_EMIT connectionDeleted(connectionId);

  return disconnected;
}


bool
CustomGraphModel::
deleteNode(NodeId const nodeId)
{
  // Delete connections to this node first.
  auto connectionIds = allConnectionIds(nodeId);
  for (auto & cId : connectionIds)
  {
    deleteConnection(cId);
  }

  _nodeIds.erase(nodeId);
  _nodeGeometryData.erase(nodeId);

  Q_EMIT nodeDeleted(nodeId);

  return true;
}


QJsonObject
CustomGraphModel::
saveNode(NodeId const nodeId) const
{
  QJsonObject nodeJson;

  nodeJson["id"] = static_cast<qint64>(nodeId);

  {
    QPointF const pos = nodeData(nodeId, NodeRole::Position).value<QPointF>();

    QJsonObject posJson;
    posJson["x"] = pos.x();
    posJson["y"] = pos.y();
    nodeJson["position"] = posJson;
  }

  return nodeJson;
}


void
CustomGraphModel::
loadNode(QJsonObject const & nodeJson)
{
  NodeId restoredNodeId = static_cast<NodeId>(nodeJson["id"].toInt());

  // Next NodeId must be larger that any id existing in the graph
  _nextNodeId = std::max(restoredNodeId + 1, _nextNodeId);

  // Create new node.
  _nodeIds.insert(restoredNodeId);

  Q_EMIT nodeCreated(restoredNodeId);

  {
    QJsonObject posJson = nodeJson["position"].toObject();
    QPointF const pos(posJson["x"].toDouble(),
                      posJson["y"].toDouble());

    setNodeData(restoredNodeId,
                NodeRole::Position,
                pos);
  }
}


QJsonObject
CustomGraphModel::
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
CustomGraphModel::
loadConnection(QJsonObject const & connJson)
{
  ConnectionId connId{static_cast<NodeId>(connJson["outNodeId"].toInt()),
                      static_cast<PortIndex>(connJson["outPortIndex"].toInt()),
                      static_cast<NodeId>(connJson["intNodeId"].toInt()),
                      static_cast<PortIndex>(connJson["inPortIndex"].toInt())};

  addConnection(connId);
}
