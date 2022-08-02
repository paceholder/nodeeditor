#pragma once

#include <nodes/GraphModel>
#include <nodes/StyleCollection>
#include <nodes/ConnectionIdUtils>


using ConnectionId     = QtNodes::ConnectionId;
using ConnectionPolicy = QtNodes::ConnectionPolicy;
using NodeFlag        = QtNodes::NodeFlag;
using NodeFlags       = QtNodes::NodeFlags;
using NodeId          = QtNodes::NodeId;
using NodeRole        = QtNodes::NodeRole;
using PortIndex       = QtNodes::PortIndex;
using PortRole        = QtNodes::PortRole;
using PortType        = QtNodes::PortType;
using StyleCollection = QtNodes::StyleCollection;
using QtNodes::InvalidNodeId;


class CustomGraphModel : public QtNodes::GraphModel
{
public:

  struct NodeGeometryData
  {
    QSize size;
    QPointF pos;
  };


public:

  CustomGraphModel()
    : _lastNodeId{0}
  {}


  std::unordered_set<NodeId>
  allNodeIds() const override
  {
    return _nodeIds;
  }

  std::unordered_set<std::pair<NodeId, PortIndex>>
  connectedNodes(NodeId    nodeId,
                 PortType  portType,
                 PortIndex portIndex) const override
  {
    return _connectivity[std::make_tuple(nodeId,
                                         portType,
                                         portIndex)];
  }

  NodeId
  addNode(QString const nodeType = QString()) override
  {
    NodeId newId = _lastNodeId++;
    // Create new node.
    _nodeIds.insert(newId);

    Q_EMIT nodeCreated(newId);

    return newId;
  }

  void
  addConnection(ConnectionId const connectionId) override
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

  QVariant
  nodeData(NodeId nodeId, NodeRole role) const override
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
  setNodeData(NodeId   nodeId,
              NodeRole role,
              QVariant value) override
  {
    bool result = false;

    switch (role)
    {
      case NodeRole::Type:
        break;
      case NodeRole::Position:
        {
          _nodeGeometryData[nodeId].pos = value.value<QPointF>();

          Q_EMIT nodePositonUpdated(nodeId);

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
  portData(NodeId    nodeId,
           PortType  portType,
           PortIndex portIndex,
           PortRole  role) const override
  {
    switch (role)
    {
      case PortRole::Data:
        return QVariant();
        break;

      case PortRole::DataType:
        return QVariant();
        break;

      case PortRole::ConnectionPolicy:
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
  setPortData(NodeId    nodeId,
              PortType  portType,
              PortIndex portIndex,
              PortRole  role) const override
  {
    Q_UNUSED(nodeId);
    Q_UNUSED(portType);
    Q_UNUSED(portIndex);
    Q_UNUSED(role);

    return false;
  }

  bool
  deleteConnection(ConnectionId const connectionId) override
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
  deleteNode(NodeId const nodeId) override
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

private:

  std::unordered_set<NodeId> _nodeIds;

  mutable
  std::unordered_map<std::tuple<NodeId, PortType, PortIndex>,
                     std::unordered_set<std::pair<NodeId, PortIndex>>>
  _connectivity;

  mutable std::unordered_map<NodeId,
                             NodeGeometryData>
  _nodeGeometryData;


  unsigned int _lastNodeId;

};
