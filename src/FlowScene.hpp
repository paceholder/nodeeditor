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

class FlowScene : public QGraphicsScene
{
public:
  static
  FlowScene& instance();

  /// TODO: make a free function
  static
  Node* locateNodeAt(QGraphicsSceneMouseEvent* event);

public:

  Connection* createConnection();

  void deleteConnection(Connection* c);

  QUuid createNode();

public:

  Connection* getConnection(QUuid id) const;

  Node* getNode(QUuid id) const;

private:
  FlowScene();
  ~FlowScene();

private:
  static FlowScene* _instance;

  std::unordered_map<QUuid, Connection*> _connections;
  std::unordered_map<QUuid, Node*>       _nodes;
};
