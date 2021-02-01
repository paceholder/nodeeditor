#include "GraphModel.hpp"

#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtCore/QVariant>
#include <QtWidgets/QWidget>

#include "ConnectionIdHash.hpp"
#include "ConnectionIdUtils.hpp"
#include "NodeData.hpp"
#include "StyleCollection.hpp"


Q_DECLARE_METATYPE(QWidget*)

namespace QtNodes
{

std::unordered_set<NodeId>
GraphModel::
allNodeIds() const
{
  std::unordered_set<NodeId> r = {1u, };

  return r;
}


std::unordered_set<ConnectionId>
GraphModel::
allConnectionIds(NodeId const nodeId) const
{
  std::unordered_set<ConnectionId> result;

  unsigned int nIn =
    nodeData(nodeId, NodeRole::NumberOfInPorts).toUInt();

  for (PortIndex i = 0; i < nIn; ++i)
  {
    auto const & connections =
      connectedNodes(nodeId, PortType::In, i);

    for (auto & cn : connections)
    {
      result.insert(std::make_tuple(cn.first, cn.second, nodeId, i));
    }

  }

  unsigned int nOut =
    nodeData(nodeId, NodeRole::NumberOfOutPorts).toUInt();

  for (PortIndex i = 0; i < nOut; ++i)
  {
    auto const & connections =
      connectedNodes(nodeId, PortType::Out, i);

    for (auto & cn : connections)
    {
      result.insert(std::make_tuple(nodeId, i, cn.first, cn.second));
    }
  }

  return result;
}


std::unordered_set<std::pair<NodeId, PortIndex>>
GraphModel::
connectedNodes(NodeId    nodeId,
               PortType  portType,
               PortIndex portIndex) const
{
  Q_UNUSED(nodeId);
  Q_UNUSED(portType);
  Q_UNUSED(portIndex);

  // No connected nodes in the default implementation.
  return std::unordered_set<std::pair<PortIndex, NodeId>>();
}


bool
GraphModel::
connectionExists(ConnectionId const connectionId) const
{
  Q_UNUSED(connectionId);

  return false;
}


NodeId
GraphModel::
addNode(QString const nodeType)
{
  return InvalidNodeId;
}


bool
GraphModel::
connectionPossible(ConnectionId const connectionId)
{
  NodeId nodeIdIn = getNodeId(PortType::In, connectionId);
  PortIndex portIndexIn = getPortIndex(PortType::In, connectionId);

  NodeDataType typeIn =
    portData(nodeIdIn,
             PortType::In,
             portIndexIn,
             PortRole::DataType).value<NodeDataType>();

  NodeId nodeIdOut = getNodeId(PortType::Out, connectionId);
  PortIndex portIndexOut = getPortIndex(PortType::Out, connectionId);

  NodeDataType typeOut =
    portData(nodeIdOut,
             PortType::Out,
             portIndexOut,
             PortRole::DataType).value<NodeDataType>();

  return (typeIn.id == typeOut.id);
}


void
GraphModel::
addConnection(ConnectionId const connectionId)
{
  Q_UNUSED(connectionId);

 Q_EMIT connectionCreated(connectionId);
}


bool
GraphModel::
nodeExists(NodeId const nodeId) const
{
  Q_UNUSED(nodeId);

  return false;
}


QVariant
GraphModel::
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
      result = QPointF(0, 0); // _position;
      break;

    case NodeRole::Size:
      result = QSize(100, 100);
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


NodeFlags
GraphModel::
nodeFlags(NodeId nodeId) const
{
  Q_UNUSED(nodeId);

  return NodeFlag::NoFlags;
}


bool
GraphModel::
setNodeData(NodeId nodeId, NodeRole role, QVariant value)
{
  Q_UNUSED(nodeId);
  Q_UNUSED(role);
  Q_UNUSED(value);

  return false;
}


QVariant
GraphModel::
portData(NodeId    nodeId,
         PortType  portType,
         PortIndex portIndex,
         PortRole  role) const
{
  Q_UNUSED(nodeId);
  Q_UNUSED(portIndex);

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
GraphModel::
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
GraphModel::
deleteConnection(ConnectionId const connectionId)
{
  Q_UNUSED(connectionId);

  if (connectionExists(connectionId))
  {
    Q_EMIT connectionDeleted(connectionId);

    return true;
  }

  return false;
}


bool
GraphModel::
deleteNode(NodeId const nodeId)
{
  Q_UNUSED(nodeId);

  if (nodeExists(nodeId))
  {
    Q_EMIT nodeDeleted(nodeId);

    return true;
  }

  return false;
}


}
