#include "FlowScene.hpp"

#include "FlowItem.hpp"
#include "FlowItemInterface.hpp"

#include <iostream>

FlowScene* FlowScene::_instance = nullptr;

FlowScene&
FlowScene::
instance()
{
  static FlowScene flowScene;

  return flowScene;
}


QUuid
FlowScene::
createConnection(std::pair<QUuid, int> address,
                 Connection::EndType draggingEnd)
{
  Connection* connection = new Connection(address, draggingEnd);

  std::cout << "CREATE CONNECTION WITH D END " << draggingEnd << std::endl;

  // add to map
  _connections[connection->id()] = connection;

  return connection->id();
}


QUuid
FlowScene::
createFlowItem()
{

  for (auto i : {1, 2, 3, 4, 5, 6, 7, 8})
  {
    (void)i;

    FlowItem* flowItem = new FlowItem();

    this->addItem(flowItem);
    _flowItems[flowItem->id()] = flowItem;

    flowItem->initializeFlowItem();
  }

  return QUuid();
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
setDraggingConnection(QUuid id, Connection::EndType dragging)
{
  _draggingConnectionID = id;
  _dragging = dragging;

  Connection* c = _connections[id];
  c->setDragging(dragging);
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
