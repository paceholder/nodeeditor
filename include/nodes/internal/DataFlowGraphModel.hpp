#pragma once

#include <memory>

#include "ConnectionIdUtils.hpp"
#include "DataModelRegistry.hpp"
#include "Export.hpp"
#include "GraphModel.hpp"
#include "StyleCollection.hpp"


namespace QtNodes
{

class NODE_EDITOR_PUBLIC DataFlowGraphModel : public GraphModel
{
  Q_OBJECT

public:

  struct NodeGeometryData
  {
    QSize size;
    QPointF pos;
  };

public:

  DataFlowGraphModel(std::shared_ptr<DataModelRegistry> registry);

  std::shared_ptr<DataModelRegistry>
  dataModelRegistry() { return _registry; }

public:

  std::unordered_set<NodeId>
  allNodeIds() const override;

  std::unordered_set<std::pair<NodeId, PortIndex>>
  connectedNodes(NodeId nodeId,
                 PortType portType,
                 PortIndex portIndex) const override;

  NodeId
  addNode(QString const nodeType) override;

  void
  addConnection(ConnectionId const connectionId) override;

  QVariant
  nodeData(NodeId nodeId, NodeRole role) const override;


  NodeFlags
  nodeFlags(NodeId nodeId) const override;

  bool
  setNodeData(NodeId nodeId,
              NodeRole role,
              QVariant value) override;

  QVariant
  portData(NodeId nodeId,
           PortType portType,
           PortIndex portIndex,
           PortRole role) const override;

  bool
  setPortData(NodeId nodeId,
              PortType portType,
              PortIndex portIndex,
              PortRole role) const override;

  bool
  deleteConnection(ConnectionId const connectionId) override;

  bool
  deleteNode(NodeId const nodeId) override;

private:

  NodeId
  newNodeId() { return _nextNodeId++; }

private Q_SLOTS:

  void
  onNodeDataUpdated(NodeId const nodeId,
                    PortIndex const portIndex);


  void
  propagateEmptyDataTo(NodeId const nodeId,
                       PortIndex const portIndex);

private:

  std::shared_ptr<DataModelRegistry> _registry;

  NodeId _nextNodeId;

  std::unordered_map<NodeId,
                     std::unique_ptr<NodeDataModel>>
  _models;

  mutable
  std::unordered_map<std::tuple<NodeId, PortType, PortIndex>,
                     std::unordered_set<std::pair<NodeId, PortIndex>>>
  _connectivity;

  mutable std::unordered_map<NodeId, NodeGeometryData>
  _nodeGeometryData;
};


}
