#include "FlowScene.hpp"

#include "FlowItem.hpp"
#include "FlowItemInterface.hpp"

FlowScene* FlowScene::_instance = nullptr;

FlowScene*
FlowScene::
instance()
{
  if (_instance == nullptr)
    _instance = new FlowScene();

  return _instance;
}

void
FlowScene::
registerFlowItem(FlowItemInterface* flowItemIterface)
{
  //
}

QUuid
FlowScene::
createConnection(QUuid                flowItemID,
                 int                  entryNumber,
                 Connection::Dragging dragging)
{
  Connection* connection =
    new Connection(flowItemID, entryNumber, dragging);

  this->addItem(connection);
  _connections[connection->id()] = connection;

  connection->initializeConnection();

  FlowItem* item = FlowScene::instance()->getFlowItem(flowItemID);

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

FlowScene::
FlowScene()
{
  //
}
