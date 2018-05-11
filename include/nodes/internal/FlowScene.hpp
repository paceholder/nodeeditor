#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>

#include <unordered_map>
#include <tuple>
#include <functional>

#include "QUuidStdHash.hpp"
#include "Export.hpp"
#include "DataModelRegistry.hpp"
#include "TypeConverter.hpp"
#include "memory.hpp"

namespace QtNodes
{

class NodeDataModel;
class FlowItemInterface;
class Node;
class NodeGraphicsObject;
class Connection;
class ConnectionGraphicsObject;
class NodeStyle;

/// Scene holds connections and nodes.
class NODE_EDITOR_PUBLIC FlowScene
  : public QGraphicsScene
{
  Q_OBJECT
public:

  FlowScene(std::shared_ptr<DataModelRegistry> registry,
            QObject * parent = Q_NULLPTR);

  FlowScene(QObject * parent = Q_NULLPTR);

  ~FlowScene();

public:

  std::shared_ptr<Connection>
  createConnection(PortType connectedPort,
                   Node& node,
                   PortIndex portIndex);

  std::shared_ptr<Connection>
  createConnection(Node& nodeIn,
                   PortIndex portIndexIn,
                   Node& nodeOut,
                   PortIndex portIndexOut,
                   TypeConverter const & converter = TypeConverter{});

  std::shared_ptr<Connection> restoreConnection(QJsonObject const &connectionJson);

  void deleteConnection(Connection& connection);

  Node&createNode(std::unique_ptr<NodeDataModel> && dataModel);

  Node&restoreNode(QJsonObject const& nodeJson);

  void removeNode(Node& node);

  DataModelRegistry&registry() const;

  void setRegistry(std::shared_ptr<DataModelRegistry> registry);

  void iterateOverNodes(std::function<void(Node*)> const & visitor);

  void iterateOverNodeData(std::function<void(NodeDataModel*)> const & visitor);

  void iterateOverNodeDataDependentOrder(std::function<void(NodeDataModel*)> const & visitor);

  QPointF getNodePosition(Node const& node) const;

  void setNodePosition(Node& node, QPointF const& pos) const;

  QSizeF getNodeSize(Node const& node) const;

public:

  std::unordered_map<QUuid, std::unique_ptr<Node> > const & nodes() const;

  std::unordered_map<QUuid, std::shared_ptr<Connection> > const & connections() const;

  std::vector<Node*> allNodes() const;

  std::vector<Node*> selectedNodes() const;

public:

  void clearScene();

  void save() const;

  void load();

  QByteArray saveToMemory() const;

  void loadFromMemory(const QByteArray& data);

Q_SIGNALS:

  /**
   * @brief Node has been created but not on the scene yet.
   * @see nodePlaced()
   */
  void nodeCreated(Node &n);

  /**
   * @brief Node has been added to the scene.
   * @details Connect to this signal if need a correct position of node.
   * @see nodeCreated()
   */
  void nodePlaced(Node &n);

  void nodeDeleted(Node &n);

  void connectionCreated(Connection const &c);
  void connectionDeleted(Connection const &c);

  void nodeMoved(Node& n, const QPointF& newLocation);

  void nodeDoubleClicked(Node& n);

  void connectionHovered(Connection& c, QPoint screenPos);

  void nodeHovered(Node& n, QPoint screenPos);

  void connectionHoverLeft(Connection& c);

  void nodeHoverLeft(Node& n);

  void nodeContextMenu(Node& n, const QPointF& pos);

private:

  using SharedConnection = std::shared_ptr<Connection>;
  using UniqueNode       = std::unique_ptr<Node>;

  // DO NOT reorder this member to go after the others.
  // This should outlive all the connections and nodes of
  // the graph, so that nodes can potentially have pointers into it,
  // which is why it comes first in the class.
  std::shared_ptr<DataModelRegistry> _registry;

  std::unordered_map<QUuid, SharedConnection> _connections;
  std::unordered_map<QUuid, UniqueNode>       _nodes;

private Q_SLOTS:

  void setupConnectionSignals(Connection const& c);

  void sendConnectionCreatedToNodes(Connection const& c);
  void sendConnectionDeletedToNodes(Connection const& c);

};

Node*
locateNodeAt(QPointF scenePoint, FlowScene &scene,
             QTransform const & viewTransform);
}
