#pragma once


#include <QtCore/QObject>
#include <QtCore/QUuid>

#include <QtCore/QJsonObject>

#include "PortType.hpp"

#include "Export.hpp"
#include "NodeState.hpp"
#include "NodeGeometry.hpp"
#include "NodeData.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "Serializable.hpp"
#include "memory.hpp"

namespace QtNodes
{

class Connection;
class ConnectionState;
class NodeGraphicsObject;
class NodeDataModel;
class NodeGroup;

/**
 * @brief The Node class stores the logical structure of a node.
 */
class NODE_EDITOR_PUBLIC Node
  : public QObject
  , public Serializable
{
  Q_OBJECT

  friend class FlowScene;

public:

  /// NodeDataModel should be an rvalue and is moved into the Node
  Node(std::unique_ptr<NodeDataModel> && dataModel);

  virtual
  ~Node();

public:

  QJsonObject
  save() const override;

  void
  restore(QJsonObject const &json) override;

  /**
   * @brief Method that restores only the ID of the node from a JSON object.
   * @param json JSON object containing the node's parameters.
   */
  void
  restoreID(QJsonObject const &json);

  void
  clone(QJsonObject const &json);

public:

  QUuid
  id() const;

  void
  reactToPossibleConnection(PortType,
                            NodeDataType const &,
                            QPointF const & scenePoint);

  void
  resetReactionToConnection();

public:

  NodeGraphicsObject const &
  nodeGraphicsObject() const;

  NodeGraphicsObject &
  nodeGraphicsObject();

  void
  setGraphicsObject(std::unique_ptr<NodeGraphicsObject>&& graphics);

  void
  setNodeGroup(std::shared_ptr<NodeGroup> group);

  void
  unsetNodeGroup();

  NodeGeometry&
  nodeGeometry();

  NodeGeometry const&
  nodeGeometry() const;

  NodeState const &
  nodeState() const;

  NodeState &
  nodeState();

  NodeDataModel*
  nodeDataModel() const;

  std::weak_ptr<NodeGroup>
  nodeGroup();

  bool isInGroup() const;

public Q_SLOTS: // data propagation

  /// Propagates incoming data to the underlying model.
  void
  propagateData(std::shared_ptr<NodeData> nodeData,
                PortIndex inPortIndex) const;

  /// Fetches data from model's OUT #index port
  /// and propagates it to the connection
  void
  onDataUpdated(PortIndex index);

  /// update the graphic part if the size of the embeddedwidget changes
  void
  onNodeSizeUpdated();

private:

  // addressing

  QUuid _uid;

  std::weak_ptr<NodeGroup> _nodeGroup{};

  // data

  std::unique_ptr<NodeDataModel> _nodeDataModel;

  NodeState _nodeState;

  // painting

  NodeGeometry _nodeGeometry;

  std::unique_ptr<NodeGraphicsObject> _nodeGraphicsObject;
};
}
