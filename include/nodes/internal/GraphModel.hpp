#pragma once

#include "Export.hpp"

#include <unordered_set>
#include <unordered_map>

#include <QtCore/QObject>
#include <QtCore/QVariant>

#include "Definitions.hpp"
#include "ConnectionIdHash.hpp"


namespace QtNodes
{

/**
 * The central class in the Model-View approach. It delivers all kinds
 * of information from the backing user data structures that represent
 * the graph. The class allows to modify the graph structure: create
 * and remove nodes and connections.
 *
 * We use two types of the unique ids for graph manipulations:
 *   - NodeId
 *   - ConnectionId
 */
class NODE_EDITOR_PUBLIC GraphModel : public QObject
{
  Q_OBJECT
public:
  /// @brief Returns the full set of unique Node Ids.
  /**
   * Model creator is responsible for generating unique `unsigned int`
   * Ids for all the nodes in the graph. From an Id it should be
   * possible to trace back to the model's internal representation of
   * the node.
   */
  virtual
  std::unordered_set<NodeId>
  allNodeIds() const;

  virtual
  std::unordered_set<ConnectionId>
  allConnectionIds(NodeId const nodeId) const;

  /// @brief Returns all connected Node Ids for given port.
  /**
   * The returned set of nodes and port indices correspond to the type
   * opposite to the given `portType`.
   */
  virtual
  std::unordered_set<std::pair<NodeId, PortIndex>>
  connectedNodes(NodeId nodeId,
                 PortType portType,
                 PortIndex index) const;


  /// Checks if two nodes with the given `connectionId` are connected.
  virtual
  bool
  connectionExists(ConnectionId const connectionId) const;


  /// Creates a new node instance in the derived class.
  /**
   * The model is responsile for generating a unique `NodeId`.
   * @param[in] nodeType is free to be used and interpreted by the
   * model on its own, it helps to distinguish between possible node
   * types and create a correct instance inside.
   *
   * Default implementation returns `InvalidNodeId`.
   */
  virtual
  NodeId
  addNode(QString const nodeType = QString());

  /// Model decides if a conection with a given connection Id possible.
  /**
   * The default implementation compares corresponding data types.
   *
   * It is possible to override the function and connect non-equal
   * data types.
   */
  virtual
  bool
  connectionPossible(ConnectionId const connectionId);

  /// Creates a new connection between two nodes.
  /**
   * Default implementation emits signal
   * `connectionCreated(connectionId)`
   *
   * In the derived classes user must emite the signal to notify the
   * scene about the changes.
   */
  virtual
  void
  addConnection(ConnectionId const connectionId);

  /**
   * @returns `true` if there is data in the model associated with the
   * given `nodeId`.
   */
  virtual
  bool
  nodeExists(NodeId const nodeId) const;


  /// @brief Returns node-related data for requested NodeRole.
  /**
   * @returns Node Caption, Node Caption Visibility, Node Position etc.
   */
  virtual
  QVariant
  nodeData(NodeId nodeId, NodeRole role) const;

  virtual
  NodeFlags
  nodeFlags(NodeId nodeId) const;

  /// @brief Sets node properties.
  /**
   * Sets: Node Caption, Node Caption Visibility,
   * Shyle, State, Node Position etc.
   * @see NodeRole.
   */
  virtual
  bool
  setNodeData(NodeId nodeId, NodeRole role, QVariant value);

  /// @brief Returns port-related data for requested NodeRole.
  /**
   * @returns Port Data Type, Port Data, Connection Policy, Port
   * Caption.
   */
  virtual
  QVariant
  portData(NodeId nodeId,
           PortType portType,
           PortIndex index,
           PortRole role) const;

  virtual
  bool
  setPortData(NodeId nodeId,
              PortType portType,
              PortIndex index,
              PortRole role) const;

  virtual
  bool
  deleteConnection(ConnectionId const connectionId);

  virtual
  bool
  deleteNode(NodeId const nodeId);

Q_SIGNALS:

  void
  connectionCreated(ConnectionId const connectionId);

  void
  connectionDeleted(ConnectionId const connectionId);

  void
  nodeCreated(NodeId const nodeId);

  void
  nodeDeleted(NodeId const nodeId);

  void
  nodePositonUpdated(NodeId const nodeId);

  void
  portDataSet(NodeId const nodeId,
              PortType const portType,
              PortIndex const portIndex);

  /**
   * Signal emitted when model is about to remove port-related data.
   * Clients must destroy existing connections to these ports.
   */
  void
  portsAboutToBeDeleted(NodeId const nodeId,
                        PortType const portType,
                        std::unordered_set<PortIndex> const &portIndexSet);

  /**
   * Signal emitted when model no longer has the old data associated
   * with the given port indices.
   */
  void
  portsDeleted(NodeId const nodeId,
               PortType const portType,
               std::unordered_set<PortIndex> const &portIndexSet);

  /**
   * Signal emitted when model is about to create new port-related
   * data.
   */
  void
  portsAboutToBeInserted(NodeId const nodeId,
                         PortType const portType,
                         std::unordered_set<PortIndex> const &portIndexSet);

  /**
   * Signal emitted when model is ready to provide the new data for
   * just creted ports. Clients must re-draw the nodes, move existing
   * conection ends to their new positions.
   */
  void
  portsInserted(NodeId const nodeId,
                PortType const portType,
                std::unordered_set<PortIndex> const &portIndexSet);
};

}
