#include "CustomGraphModel.hpp"


CustomGraphModel::
CustomGraphModel()
  : _lastNodeId{0}
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


std::unordered_set<std::pair<NodeId, PortIndex>>
CustomGraphModel::
connectedNodes(NodeId    nodeId,
               PortType  portType,
               PortIndex portIndex) const
{
  std::unordered_set<std::pair<NodeId, PortIndex>> result;

  auto connectivityKey = std::make_tuple(nodeId, portType, portIndex);

  auto it = _connectivity.find(connectivityKey);

  if (it != _connectivity.end())
    return it->second;

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
  NodeId newId = _lastNodeId++;
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
