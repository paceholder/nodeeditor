#pragma once

#include <QtCore/QUuid>

#include <unordered_map>

#include <QtWidgets/QGraphicsScene>

#include <tuple>
#include <memory>

#include "Connection.hpp"

namespace std
{
template<>
struct hash<QUuid>
{
  inline
  size_t operator()(QUuid const& uid) const
  {
    return qHash(uid);
  }
};
}

class NodeDataModel;
class FlowItemInterface;
class Node;

/// Scene holds connections and nodes.
class FlowScene : public QGraphicsScene
{
public:

  FlowScene();

  ~FlowScene();

public:

  std::shared_ptr<Connection>
  createConnection(PortType connectedPort,
                   std::shared_ptr<Node> node,
                   PortIndex portIndex);

  void deleteConnection(std::shared_ptr<Connection> connection);

  std::shared_ptr<Node>
  createNode(std::unique_ptr<NodeDataModel> &&dataModel);

private:

  using SharedConnection = std::shared_ptr<Connection>;
  using SharedNode       = std::shared_ptr<Node>;

  std::unordered_map<QUuid, SharedConnection> _connections;
  std::unordered_map<QUuid, SharedNode>       _nodes;
};

std::shared_ptr<Node>
locateNodeAt(QPointF scenePoint, FlowScene &scene, QTransform viewTransform);
