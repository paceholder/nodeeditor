#pragma once

#include "ConnectionIdUtils.hpp"
#include "NodeDelegateModelRegistry.hpp"
#include "Export.hpp"
#include "AbstractGraphModel.hpp"
#include "StyleCollection.hpp"

#include <QJsonObject>

#include <memory>

namespace QtNodes
{

class NODE_EDITOR_PUBLIC DataFlowGraphModel : public AbstractGraphModel
{
  Q_OBJECT

public:
  struct NodeGeometryData
  {
    QSize size;
    QPointF pos;
  };

public:
  DataFlowGraphModel(std::shared_ptr<NodeDelegateModelRegistry> registry);

  std::shared_ptr<NodeDelegateModelRegistry>
  dataModelRegistry() { return _registry; }

public:
  std::unordered_set<NodeId>
  allNodeIds() const override;

  std::unordered_set<ConnectionId>
  allConnectionIds(NodeId const nodeId) const override;

  std::unordered_set<ConnectionId>
  connections(NodeId    nodeId,
              PortType  portType,
              PortIndex portIndex) const override;

  bool
  connectionExists(ConnectionId const connectionId) const override;

  NodeId
  addNode(QString const nodeType) override;

  bool
  connectionPossible(ConnectionId const connectionId) const override;

  void
  addConnection(ConnectionId const connectionId) override;

  bool
  nodeExists(NodeId const nodeId) const override;

  QVariant
  nodeData(NodeId nodeId, NodeRole role) const override;


  NodeFlags
  nodeFlags(NodeId nodeId) const override;

  bool
  setNodeData(NodeId   nodeId,
              NodeRole role,
              QVariant value) override;

  QVariant
  portData(NodeId    nodeId,
           PortType  portType,
           PortIndex portIndex,
           PortRole  role) const override;

  bool
  setPortData(NodeId          nodeId,
              PortType        portType,
              PortIndex       portIndex,
              QVariant const& value,
              PortRole        role = PortRole::Data) override;

  bool
  deleteConnection(ConnectionId const connectionId) override;

  bool
  deleteNode(NodeId const nodeId) override;

  QJsonObject
  saveNode(NodeId const) const override;

  QJsonDocument
  save() const;

  void
  loadNode(QJsonObject const & nodeJson) override;

  void
  load(QJsonDocument const &json);

  QJsonObject
  saveConnection(ConnectionId const & connId) const override;

  void
  loadConnection(QJsonObject const & connJson) override;

  /**
   * Fetches the NodeDelegateModel for the given `nodeId` and tries to cast the
   * stored pointer to the given type
   */
  template<typename NodeDelegateModelType>
  NodeDelegateModelType*
  delegateModel(NodeId const nodeId)
  {
    auto it = _models.find(nodeId);
    if (it == _models.end())
      return nullptr;

    auto model = dynamic_cast<NodeDelegateModelType*>(it->second.get());

    return model;
  }

private:
  NodeId
  newNodeId() { return _nextNodeId++; }

  /**
   * The function could be used when we restore nodes from some file
   * and the NodeId values are already known.  In this case we must
   * update internal counter for unique "next" node id in order not to
   * repeat the values when incrementing.
   */
  void
  setNextNodeId(NodeId const restoredNodeId)
  {
    _nextNodeId = std::max(_nextNodeId, restoredNodeId + 1);
  }

private Q_SLOTS:
  /**
   * Fuction is called in three cases:
   *
   * - By underlying NodeDelegateModel when a node has new data to propagate.
   *   @see DataFlowGraphModel::addNode
   * - When a new connection is created.
   *   @see DataFlowGraphModel::addConnection
   * - When a node restored from JSON an needs to send data downstream.
   *   @see DataFlowGraphModel::loadNode
   */
  void
  onOutPortDataUpdated(NodeId const    nodeId,
                       PortIndex const portIndex);


  /// Function is called after detaching a connection.
  void
  propagateEmptyDataTo(NodeId const    nodeId,
                       PortIndex const portIndex);

private:
  std::shared_ptr<NodeDelegateModelRegistry> _registry;

  NodeId _nextNodeId;

  std::unordered_map<NodeId,
                     std::unique_ptr<NodeDelegateModel>>
  _models;

  using ConnectivityKey =
    std::tuple<NodeId, PortType, PortIndex>;

  std::unordered_map<ConnectivityKey,
                     std::unordered_set<std::pair<NodeId, PortIndex>>>
  _connectivity;

  mutable std::unordered_map<NodeId, NodeGeometryData>
  _nodeGeometryData;
};


}
