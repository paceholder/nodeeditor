#include "DynamicPortsModel.hpp"

#include "PortAddRemoveWidget.hpp"

#include <QtNodes/ConnectionIdUtils>

#include <iterator>


DynamicPortsModel::
DynamicPortsModel()
  : _nextNodeId{0}
{}


DynamicPortsModel::
~DynamicPortsModel()
{
  //
}


std::unordered_set<NodeId>
DynamicPortsModel::
allNodeIds() const
{
  return _nodeIds;
}


std::unordered_set<ConnectionId>
DynamicPortsModel::
allConnectionIds(NodeId const nodeId) const
{
  std::unordered_set<ConnectionId> result;

  std::copy_if(_connectivity.begin(),
               _connectivity.end(),
               std::inserter(result, std::end(result)),
               [&nodeId](ConnectionId const & cid)
               {
                  return cid.inNodeId == nodeId ||
                         cid.outNodeId == nodeId;
               });

  return result;
}


std::unordered_set<ConnectionId>
DynamicPortsModel::
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
DynamicPortsModel::
connectionExists(ConnectionId const connectionId) const
{
  return (_connectivity.find(connectionId) != _connectivity.end());
}


NodeId
DynamicPortsModel::
addNode(QString const nodeType)
{
  NodeId newId = _nextNodeId++;
  // Create new node.
  _nodeIds.insert(newId);

  Q_EMIT nodeCreated(newId);

  return newId;
}


bool
DynamicPortsModel::
connectionPossible(ConnectionId const connectionId) const
{
  return !connectionExists(connectionId);
}


void
DynamicPortsModel::
addConnection(ConnectionId const connectionId)
{
  _connectivity.insert(connectionId);

  Q_EMIT connectionCreated(connectionId);
}


bool
DynamicPortsModel::
nodeExists(NodeId const nodeId) const
{
  return (_nodeIds.find(nodeId) != _nodeIds.end());
}


QVariant
DynamicPortsModel::
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
      result = _nodePortCounts[nodeId].in;
      break;

    case NodeRole::OutPortCount:
      result = _nodePortCounts[nodeId].out;
      break;

    case NodeRole::Widget:
    {
      auto it = _nodeWidgets.find(nodeId);
      if (it == _nodeWidgets.end())
      {
        _nodeWidgets[nodeId] =
          new PortAddRemoveWidget(_nodePortCounts[nodeId].in,
                                  _nodePortCounts[nodeId].out,
                                  nodeId,
                                  *const_cast<DynamicPortsModel*>(this));
      }
      result = QVariant::fromValue(_nodeWidgets[nodeId]);
      break;
    }
  }

  return result;
}


bool
DynamicPortsModel::
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
DynamicPortsModel::
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
DynamicPortsModel::
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
DynamicPortsModel::
deleteConnection(ConnectionId const connectionId)
{
  bool disconnected = false;

  auto it = _connectivity.find(connectionId);

  if (it != _connectivity.end())
  {
    disconnected = true;

    _connectivity.erase(it);
  };

  if (disconnected)
    Q_EMIT connectionDeleted(connectionId);

  return disconnected;
}


bool
DynamicPortsModel::
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
  _nodePortCounts.erase(nodeId);

  Q_EMIT nodeDeleted(nodeId);

  return true;
}


QJsonObject
DynamicPortsModel::
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
DynamicPortsModel::
loadNode(QJsonObject const & nodeJson)
{
  NodeId restoredNodeId = static_cast<NodeId>(nodeJson["id"].toInt());

  _nextNodeId = std::max(_nextNodeId, restoredNodeId + 1);

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


void
DynamicPortsModel::
addPort(NodeId nodeId,
        PortType portType,
        PortIndex portIndex)
{
  // STAGE 1.
  // Compute new addresses for the existing connections that are shifted and
  // placed after the new ones
  PortIndex first = portIndex;
  PortIndex last = first;
  portsAboutToBeInserted(nodeId, portType, first, last);


  // STAGE 2. Change the number of connections in your model
  if (portType == PortType::In)
    _nodePortCounts[nodeId].in++;
  else
    _nodePortCounts[nodeId].out++;


  // STAGE 3. Re-create previouly existed and now shifted connections
  portsInserted();

  Q_EMIT nodeUpdated(nodeId);
}


void
DynamicPortsModel::
removePort(NodeId nodeId,
           PortType portType,
           PortIndex portIndex)
{
  // STAGE 1.
  // Compute new addresses for the existing connections that are shifted upwards
  // instead of the deleted ports.
  PortIndex first = portIndex;
  PortIndex last = first;
  portsAboutToBeDeleted(nodeId, portType, first, last);

  // STAGE 2. Change the number of connections in your model
  if (portType == PortType::In)
    _nodePortCounts[nodeId].in--;
  else
    _nodePortCounts[nodeId].out--;

  portsDeleted();

  Q_EMIT nodeUpdated(nodeId);
}
