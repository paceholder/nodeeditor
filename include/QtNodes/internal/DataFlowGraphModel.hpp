#pragma once

#include "ConnectionIdUtils.hpp"
#include "NodeDelegateModelRegistry.hpp"
#include "Export.hpp"
#include "AbstractGraphModel.hpp"
#include "StyleCollection.hpp"

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

  std::unordered_set<std::pair<NodeId, PortIndex>>
  connectedNodes(NodeId    nodeId,
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
  setPortData(NodeId    nodeId,
              PortType  portType,
              PortIndex portIndex,
              PortRole  role) const override;

  bool
  deleteConnection(ConnectionId const connectionId) override;

  bool
  deleteNode(NodeId const nodeId) override;

private:

  NodeId
  newNodeId() { return _nextNodeId++; }

private Q_SLOTS:

  /**
   * Fuction is called by NodeDelegateModel when a node has new data to
   * propagate.
   */
  void
  onNodeDataUpdated(NodeId const    nodeId,
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
