#pragma once

#include "PortType.hpp"
#include "Export.hpp"
#include "NodeStyle.hpp"

#include <cstddef>

#include <QString>
#include <QPointF>
#include <QObject>
#include <QUuid>
#include <QList>


namespace QtNodes
{

class NodeIndex;
struct NodeDataType;
class NodePainterDelegate;

enum class ConnectionPolicy
{
  One,
  Many
};

enum class NodeValidationState
{
  Valid,
  Warning,
  Error
};

// data-type-in, data-type-out
using TypeConverterId =
  std::pair<NodeDataType, NodeDataType>;


class NODE_EDITOR_PUBLIC FlowSceneModel : public QObject
{
  Q_OBJECT

public:

  FlowSceneModel() = default;

  // Scene specific functions
  virtual QStringList
  modelRegistry() const = 0;

  /// Get the category for a node type
  /// name will be from `modelRegistry()`
  virtual QString
  nodeTypeCategory(QString const& /*name*/) const { return {}; }

  /// Get if two types are convertable
  virtual bool
  getTypeConvertable(TypeConverterId const& /*id*/) const { return false; }

  // Retrieval functions
  //////////////////////

  /// Get the list of node IDs
  virtual QList<QUuid>
  nodeUUids() const = 0;

  /// Create a NodeIndex for a node
  virtual NodeIndex
  nodeIndex(const QUuid& ID) const = 0;

  /// Get the type ID for the node
  virtual QString
  nodeTypeIdentifier(NodeIndex const& index) const = 0;

  /// Get the caption for the node
  virtual QString
  nodeCaption(NodeIndex const& index) const = 0;

  /// Get the location of a node
  virtual QPointF
  nodeLocation(NodeIndex const& index) const = 0;

  /// Get the embedded widget
  virtual QWidget*
  nodeWidget(NodeIndex const& index) const = 0;

  /// Get if it's resizable
  virtual bool
  nodeResizable(NodeIndex const& index) const = 0;

  /// Get the validation state
  virtual NodeValidationState
  nodeValidationState(NodeIndex const& index) const = 0;

  /// Get the validation error/warning
  virtual QString
  nodeValidationMessage(NodeIndex const& index) const = 0;

  /// Get the painter delegate
  virtual NodePainterDelegate*
  nodePainterDelegate(NodeIndex const& index) const = 0;

  /// Get the style - {} for default
  virtual NodeStyle
  nodeStyle(NodeIndex const& /*index*/) const { return {}; }

  /// Get the count of DataPorts
  virtual unsigned int
  nodePortCount(NodeIndex const& index, PortType portType) const = 0;

  /// Get the port caption
  virtual QString
  nodePortCaption(NodeIndex const& index, PortIndex portID, PortType portType) const = 0;

  /// Get the port data type
  virtual NodeDataType
  nodePortDataType(NodeIndex const& index, PortIndex portID, PortType portType) const = 0;

  /// Port Policy
  virtual ConnectionPolicy
  nodePortConnectionPolicy(NodeIndex const& index, PortIndex portID, PortType portType) const = 0;

  /// Get a connection at a port
  /// Returns the remote node and the remote port index for that node
  virtual std::vector<std::pair<NodeIndex, PortIndex> >
  nodePortConnections(NodeIndex const& index, PortIndex portID, PortType portTypes) const = 0;

  // Mutation functions
  /////////////////////

  /// Remove a connection
  virtual bool
  removeConnection(NodeIndex const& /*leftNode*/,
                   PortIndex /*leftPortID*/,
                   NodeIndex const& /*rightNode*/,
                   PortIndex /*rightPortID*/)
  { return false; }

  /// Add a connection
  virtual bool
  addConnection(NodeIndex const& /*leftNode*/,
                PortIndex /*leftPortID*/,
                NodeIndex const& /*rightNode*/,
                PortIndex /*rightPortID*/)
  { return false; }

  /// Remove a node
  virtual bool
  removeNode(NodeIndex const& /*index*/) { return false; }

  /// Add a  -- return {} if it fails
  virtual QUuid
  addNode(QString const& /*typeID*/, QPointF const& /*pos*/) { return QUuid{}; }

  /// Move a node to a new location
  virtual bool
  moveNode(NodeIndex const& /*index*/, QPointF /*newLocation*/) { return false; }

public:

  /// Helper functions
  ////////////////////

  // try to remove all connections and then the node
  bool
  removeNodeWithConnections(NodeIndex const& index);

public:


  /// Notifications
  /////////////////

  virtual void
  connectionHovered(NodeIndex const& /*lhs*/, PortIndex /*lPortIndex*/, NodeIndex const& /*rhs*/, PortIndex /*rPortIndex*/, QPoint const& /*pos*/, bool /*entered*/) {}

  virtual void
  nodeHovered(NodeIndex const& /*index*/, QPoint const& /*pos*/, bool /*entered*/) {}

  virtual void
  nodeDoubleClicked(NodeIndex const& /*index*/, QPoint const& /*pos*/) {}

  virtual void
  nodeContextMenu(NodeIndex const& /*index*/, QPoint const& /*pos*/) {}

signals:

  void
  nodeRemoved(const QUuid& id);
  void
  nodeAdded(const QUuid& newID);
  void
  nodePortUpdated(NodeIndex const& id);
  void
  nodeValidationUpdated(NodeIndex const& id);
  void
  connectionRemoved(NodeIndex const& leftNode, PortIndex leftPortID, NodeIndex const& rightNode, PortIndex rightPortID);
  void
  connectionAdded(NodeIndex const& leftNode, PortIndex leftPortID, NodeIndex const& rightNode, PortIndex rightPortID);
  void
  nodeMoved(NodeIndex const& index);

protected:

  NodeIndex
  createIndex(const QUuid& id, void* internalPointer) const;

};

} // namespace QtNodes
