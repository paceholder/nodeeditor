#pragma once

#include <QtCore/QUuid>

#include <unordered_map>

#include <QtWidgets/QGraphicsScene>

#include <tuple>
#include <memory>

#include "Connection.hpp"
#include "DataModelRegistry.hpp"

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

class FlowItemInterface;
class Node;

/// Scene holds connections and nodes.
class FlowScene : public QGraphicsScene
{
public:
  static
  FlowScene& instance();

public:

  std::shared_ptr<Connection> createConnection();

  void deleteConnection(QUuid const & id);

  QUuid createNode();

public:

  std::shared_ptr<Connection> getConnection(QUuid id) const;

  std::shared_ptr<Node> getNode(QUuid id) const;

private:

  FlowScene();
  ~FlowScene();

private:

  using SharedConnection = std::shared_ptr<Connection>;
  using SharedNode       = std::shared_ptr<Node>;

  std::unordered_map<QUuid, SharedConnection> _connections;
  std::unordered_map<QUuid, SharedNode>       _nodes;
};

std::shared_ptr<Node>
locateNodeAt(QGraphicsSceneMouseEvent* event);
