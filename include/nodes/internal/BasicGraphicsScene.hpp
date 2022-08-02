#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QMenu>

#include <functional>
#include <memory>
#include <tuple>
#include <unordered_map>

//#include "DataModelRegistry.hpp"
#include "Definitions.hpp"
#include "Export.hpp"
#include "GraphModel.hpp"
#include "ConnectionIdHash.hpp"
//#include "TypeConverter.hpp"

#include "QUuidStdHash.hpp"

namespace QtNodes
{

class ConnectionGraphicsObject;
class GraphModel;
class NodeGraphicsObject;
class NodeStyle;

/// Scene holds connections and nodes.
class NODE_EDITOR_PUBLIC BasicGraphicsScene : public QGraphicsScene
{
  Q_OBJECT
public:

  BasicGraphicsScene(GraphModel & graphModel,
                     QObject *    parent = nullptr);

  // Scenes without models are not supported
  BasicGraphicsScene() = delete;

  ~BasicGraphicsScene();

public:

  GraphModel const &
  graphModel() const;

  GraphModel &
  graphModel();

public:

  //ConnectionGraphicsObject *
  //draftConnection() const;

  /// Creates instance of "disconnected" ConnectionGraphicsObject.
  /**
   * We store a "draft" connection which has one loose end.
   * After attachment the "draft" instance is removed and instead a
   * normal "full" connection is created.
   * Function @returns the "draft" instance for further geometry
   * manipulations.
   */
  std::unique_ptr<ConnectionGraphicsObject> const &
  makeDraftConnection(ConnectionId const newConnectionId);

  void
  resetDraftConnection();

  void
  clearScene();

public:

  NodeGraphicsObject *
  nodeGraphicsObject(NodeId nodeId);

  ConnectionGraphicsObject *
  connectionGraphicsObject(ConnectionId connectionId);

public:

  virtual
  QMenu *
  createSceneMenu(QPointF const scenePos);

Q_SIGNALS:

  void
  connectionCreated(ConnectionId const connectionId);

  //void nodeMoved(Node& n, const QPointF& newLocation);

  void
  nodeClicked(NodeId const nodeId);

  void
  nodeDoubleClicked(NodeId const nodeId);

  void
  connectionHovered(ConnectionId const connectionId, QPoint const screenPos);

  void
  nodeHovered(NodeId const nodeId, QPoint const screenPos);

  void
  connectionHoverLeft(ConnectionId const connectionId);

  void
  nodeHoverLeft(NodeId const nodeId);

  void
  nodeContextMenu(NodeId const nodeId, QPointF const pos);

private:

  /// @brief Creates Node and Connection graphics objects.
  /** We perform depth-first graph traversal. The connections are
   * created by checking non-empyt node's Out ports.
   */
  void
  traverseGraphAndPopulateGraphicsObjects();

  void
  updateAttachedNodes(ConnectionId const connectionId,
                      PortType const     portType);

private Q_SLOTS:


  /// Deletes the object from the main connection object set.
  /**
   * The function returns a unique pointer to the graphics object. If
   * the pointer is not stored somewhere, the object is automatically
   * destroyed and removed from the scene.
   */
  void
  onConnectionDeleted(ConnectionId const connectionId);

  void
  onConnectionCreated(ConnectionId const connectionId);

  void
  onNodeDeleted(NodeId const nodeId);

  void
  onNodeCreated(NodeId const nodeId);

  void
  onNodePositionUpdated(NodeId const nodeId);

  void
  onPortsAboutToBeDeleted(NodeId const   nodeId,
                          PortType const portType,
                          std::unordered_set<PortIndex> const & portIndexSet);

  void
  onPortsDeleted(NodeId const   nodeId,
                 PortType const portType,
                 std::unordered_set<PortIndex> const & portIndexSet);

  void
  onPortsAboutToBeInserted(NodeId const   nodeId,
                           PortType const portType,
                           std::unordered_set<PortIndex> const & portIndexSet);

  void
  onPortsInserted(NodeId const   nodeId,
                  PortType const portType,
                  std::unordered_set<PortIndex> const & portIndexSet);


private:

  // TODO shared pointer?
  GraphModel & _graphModel;


  using UniqueNodeGraphicsObject =
    std::unique_ptr<NodeGraphicsObject>;

  using UniqueConnectionGraphicsObject =
    std::unique_ptr<ConnectionGraphicsObject>;

  std::unordered_map<NodeId, UniqueNodeGraphicsObject>
  _nodeGraphicsObjects;

  std::unordered_map<ConnectionId,
                     UniqueConnectionGraphicsObject>
  _connectionGraphicsObjects;


  //std::shared_ptr<DataModelRegistry>          _registry;

  std::unique_ptr<ConnectionGraphicsObject> _draftConnection;

};


}
