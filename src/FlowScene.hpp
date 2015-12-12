#ifndef _FLOW_SCENE_HPP_
#define _FLOW_SCENE_HPP_

#include <QtCore/QUuid>

#include <unordered_map>

#include <QtWidgets/QGraphicsScene>

#include <tuple>

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

class FlowItemInterface;
class Node;

class FlowScene : public QGraphicsScene
{
public:
  static
  FlowScene& instance();

  static
  Node* locateNodeAt(QGraphicsSceneMouseEvent* event);

public:
  //QUuid createConnection(std::pair<QUuid, int> address,
                         //EndType draggingEnd);

  Connection* createConnection();

  QUuid createNode();

public:

  Connection* getConnection(QUuid id) const;

  Node* getNode(QUuid id) const;

  bool isDraggingConnection();

  /// Sets currently dragging connection to empty value
  void clearDraggingConnection();

private:
  FlowScene();

private:
  //QMap<QString, FlowItemInterface*> _registeredInterfaces;

  static FlowScene* _instance;

  std::unordered_map<QUuid, Connection*> _connections;
  std::unordered_map<QUuid, Node*>       _flowItems;

  QUuid   _draggingConnectionID;
  EndType _dragging;
};

#endif //  _FLOW_SCENE_HPP_
