#ifndef _FLOW_SCENE_HPP_
#define _FLOW_SCENE_HPP_

#include <QtCore/QMap>
#include <QtCore/QUuid>

#include <QtWidgets/QGraphicsScene>

#include "Connection.hpp"

class FlowItemInterface;
class FlowItem;

class FlowScene: public QGraphicsScene
{
public:
  static
  FlowScene*
  instance();

public:
  void
  registerFlowItem(FlowItemInterface* flowItemIterface);

  QUuid
  createConnection(QUuid flowItemID, Connection::Dragging dragging);

  QUuid
  createFlowItem();

  Connection*
  getConnection(QUuid id) const;

  FlowItem*
  getFlowItem(QUuid id) const;

private:
  FlowScene();

private:
  QMap<QString, FlowItemInterface*> _registeredInterfaces;

  static FlowScene* _instance;

  QMap<QUuid, Connection*> _connections;
  QMap<QUuid, FlowItem*>   _flowItems;
};

#endif //  _FLOW_SCENE_HPP_
