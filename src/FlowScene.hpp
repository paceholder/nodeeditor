#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>

#include <unordered_map>
#include <tuple>
#include <memory>

#include "Connection.hpp"
#include "Export.hpp"
#include "DataModelRegistry.hpp"

class NodeDataModel;
class FlowItemInterface;
class Node;
class NodeGraphicsObject;
class ConnectionGraphicsObject;
class NodeStyle;

/// Scene holds connections and nodes.
class NODE_EDITOR_PUBLIC FlowScene
  : public QGraphicsScene
{
  Q_OBJECT
public:

  FlowScene(std::shared_ptr<DataModelRegistry> registry =
              std::make_shared<DataModelRegistry>());

  ~FlowScene();

public:

  std::shared_ptr<Connection>
  createConnection(PortType connectedPort,
                   std::shared_ptr<Node> node,
                   PortIndex portIndex);

  std::shared_ptr<Connection>
  createConnection(std::shared_ptr<Node> nodeIn,
                   PortIndex portIndexIn,
                   std::shared_ptr<Node> nodeOut,
                   PortIndex portIndexOut);

  std::shared_ptr<Connection>
  restoreConnection(Properties const &p);

  void
  deleteConnection(Connection& connection);

  std::shared_ptr<Node>
  createNode(std::unique_ptr<NodeDataModel> && dataModel);

  std::shared_ptr<Node>
  restoreNode(Properties const &p);

  void
  removeNode(std::shared_ptr<Node> node);

  void
  removeConnection(std::shared_ptr<Connection> connection);

  DataModelRegistry&
  registry();

  void
  setRegistry(std::shared_ptr<DataModelRegistry> registry);

public:

  void
  save() const;

  void
  load();

signals:
  void
  nodeCreated(const std::shared_ptr<Node>& n);
  void
  nodeDeleted(const std::shared_ptr<Node>& n);

  void
  connectionCreated(Connection& c);
  void
  connectionDeleted(Connection& c);

private:

  using SharedConnection = std::shared_ptr<Connection>;
  using SharedNode       = std::shared_ptr<Node>;

  std::unordered_map<QUuid, SharedConnection> _connections;
  std::unordered_map<QUuid, SharedNode>       _nodes;
  std::shared_ptr<DataModelRegistry>          _registry;
};

std::shared_ptr<Node>
locateNodeAt(QPointF scenePoint, FlowScene &scene,
             QTransform viewTransform);
