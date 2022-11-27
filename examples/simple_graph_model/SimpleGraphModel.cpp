#include "SimpleGraphModel.hpp"


SimpleGraphModel::
SimpleGraphModel()
  : _nextNodeId{0}
{}


SimpleGraphModel::
~SimpleGraphModel()
{
  //
}


std::unordered_set<NodeId>
SimpleGraphModel::
allNodeIds() const
{
  return _nodeIds;
}


std::unordered_set<ConnectionId>
SimpleGraphModel::
allConnectionIds(NodeId const nodeId) const
{
  return _connectivity;
}


std::unordered_set<ConnectionId>
SimpleGraphModel::
connections(NodeId    nodeId,
            PortType  portType,
            PortIndex portIndex) const
{
  std::unordered_set<ConnectionId> result;

  std::copy_if(_connectivity.begin(),
               _connectivity.end(),
               std::inserter(result, std::end(result)),
               [&portType, &portIndex, &nodeId](ConnectionId const & cid)
               {
                  return (getNodeId(portType, cid) == nodeId &&
                          getPortIndex(portType, cid) == portIndex);
               });

  return result;
}


bool
SimpleGraphModel::
connectionExists(ConnectionId const connectionId) const
{
  return (_connectivity.find(connectionId) != _connectivity.end());
}


NodeId
SimpleGraphModel::
addNode(QString const nodeType)
{
  NodeId newId = _nextNodeId++;
  // Create new node.
  _nodeIds.insert(newId);

  Q_EMIT nodeCreated(newId);

  return newId;
}


bool
SimpleGraphModel::
connectionPossible(ConnectionId const connectionId) const
{
  return _connectivity.find(connectionId) == _connectivity.end();
}


void
SimpleGraphModel::
addConnection(ConnectionId const connectionId)
{
  _connectivity.insert(connectionId);

  Q_EMIT connectionCreated(connectionId);
}


bool
SimpleGraphModel::
nodeExists(NodeId const nodeId) const
{
  return (_nodeIds.find(nodeId) != _nodeIds.end());
}


QVariant
SimpleGraphModel::
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
      result = style.toJson().toVariantMap();
    }
    break;

    case NodeRole::InternalData:
      break;

    case NodeRole::InPortCount:
      result = 1u;
      break;

    case NodeRole::OutPortCount:
      result = 1u;
      break;

    case NodeRole::Widget:
      result = QVariant();
      break;
  }

  return result;
}


bool
SimpleGraphModel::
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

    case NodeRole::InPortCount:
      break;

    case NodeRole::OutPortCount:
      break;

    case NodeRole::Widget:
      break;
  }

  return result;
}


QVariant
SimpleGraphModel::
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
SimpleGraphModel::
setPortData(NodeId    nodeId,
            PortType  portType,
            PortIndex portIndex,
            QVariant const& value,
            PortRole  role)
{
  Q_UNUSED(nodeId);
  Q_UNUSED(portType);
  Q_UNUSED(portIndex);
  Q_UNUSED(value);
  Q_UNUSED(role);

  return false;
}


bool
SimpleGraphModel::
deleteConnection(ConnectionId const connectionId)
{
  bool disconnected = false;

  auto it = _connectivity.find(connectionId);

  if (it != _connectivity.end())
  {
    disconnected = true;

    _connectivity.erase(it);
  }

  if (disconnected)
    Q_EMIT connectionDeleted(connectionId);

  return disconnected;
}


bool
SimpleGraphModel::
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
SimpleGraphModel::
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
SimpleGraphModel::
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
SimpleGraphModel::
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
SimpleGraphModel::
loadConnection(QJsonObject const & connJson)
{
  ConnectionId connId{static_cast<NodeId>(connJson["outNodeId"].toInt()),
                      static_cast<PortIndex>(connJson["outPortIndex"].toInt()),
                      static_cast<NodeId>(connJson["intNodeId"].toInt()),
                      static_cast<PortIndex>(connJson["inPortIndex"].toInt())};

  addConnection(connId);
}
