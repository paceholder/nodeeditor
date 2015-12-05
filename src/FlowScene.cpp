#include "FlowScene.hpp"

#include "FlowItem.hpp"
#include "FlowItemInterface.hpp"

FlowScene* FlowScene::_instance = nullptr;

FlowScene&
FlowScene::
instance()
{
  static FlowScene flowScene;

  return flowScene;
}


void
FlowScene::
registerFlowItem(FlowItemInterface* flowItemIterface)
{
  //
}


QUuid
FlowScene::
createConnection(QUuid flowItemID,
                 int entryNumber,
                 Connection::Dragging dragging)
{
  Connection* connection =
    new Connection(flowItemID, entryNumber, dragging);

  // add to scene
  this->addItem(connection);

  // add to map
  _connections[connection->id()] = connection;

  connection->initializeConnection();

  FlowItem* item = FlowScene::instance().getFlowItem(flowItemID);

  // z-ordering
  connection->stackBefore(item);

  return connection->id();
}


QUuid
FlowScene::
createFlowItem()
{
  FlowItem* flowItem = new FlowItem();

  this->addItem(flowItem);
  _flowItems[flowItem->id()] = flowItem;

  flowItem->initializeFlowItem();

  ////

  flowItem = new FlowItem();

  this->addItem(flowItem);
  _flowItems[flowItem->id()] = flowItem;

  flowItem->initializeFlowItem();

  return flowItem->id();
}


Connection*
FlowScene::
getConnection(QUuid id) const
{
  return _connections[id];
}


FlowItem*
FlowScene::
getFlowItem(QUuid id) const
{
  return _flowItems[id];
}


void
FlowScene::
setDraggingConnection(QUuid id, Connection::Dragging dragging)
{
  _draggingConnectionID = id;
  _dragging = dragging;

  Connection* c = _connections[id];
  c->setDragging(dragging);
}


std::pair<QUuid, Connection::Dragging>
FlowScene::
getDraggingConnection(QUuid& id, Connection::Dragging& dragging) const
{
  //
}


void
FlowScene::
clearDraggingConnection()
{
  _draggingConnectionID = QUuid();
  _dragging = Connection::NONE;
}


bool
FlowScene::
isDraggingConnection()
{
  return !_draggingConnectionID.isNull();
}


FlowScene::
FlowScene() :
  _draggingConnectionID(QUuid()),
  _dragging(Connection::NONE)
{
  //
}
