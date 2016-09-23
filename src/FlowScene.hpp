#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsScene>

#include <unordered_map>
#include <tuple>
#include <memory>

#include "Connection.hpp"
#include "Export.hpp"

namespace std
{
template<>
struct hash<QUuid>
{
  inline
  size_t
  operator()(QUuid const& uid) const
  {
    return qHash(uid);
  }
};
}

class NodeDataModel;
class FlowItemInterface;
class Node;

/// Scene holds connections and nodes.
class NODE_EDITOR_PUBLIC FlowScene
  : public QGraphicsScene
{
public:

  FlowScene();

  ~FlowScene();

public:

  std::shared_ptr<Connection>
  createConnection(PortType connectedPort,
                   std::shared_ptr<Node> node,
                   PortIndex portIndex);

  std::shared_ptr<Connection>
  restoreConnection(Properties const &p);

  void
  deleteConnection(std::shared_ptr<Connection> connection);

  std::shared_ptr<Node>
  createNode(std::unique_ptr<NodeDataModel> && dataModel);

  std::shared_ptr<Node>
  restoreNode(Properties const &p);

  void
  removeNode(QGraphicsItem* item);

  void
  save() const;

  void
  load();

private:

  using SharedConnection = std::shared_ptr<Connection>;
  using SharedNode       = std::shared_ptr<Node>;

  std::unordered_map<QUuid, SharedConnection> _connections;
  std::unordered_map<QUuid, SharedNode>       _nodes;
};

std::shared_ptr<Node>
locateNodeAt(QPointF scenePoint, FlowScene &scene, QTransform viewTransform);
