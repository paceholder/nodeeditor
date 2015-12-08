#include "FlowScene.hpp"

#include "Node.hpp"
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

  // add to map
  _connections[connection->id()] = connection;

  return connection->id();
}


QUuid
FlowScene::
createNode()
{

  //for (auto i : {1, 2, 3, 4, 5, 6, 7, 8})
  for (auto i : {1, 2})
  {
    (void)i;

    Node* flowItem = new Node();

    this->addItem(flowItem);

    _flowItems[flowItem->id()] = flowItem;

    flowItem->initializeNode();


    flowItem->moveBy(std::rand() % 100, std::rand() % 100);
  }

  return QUuid();
}


Connection*
FlowScene::
getConnection(QUuid id) const
{
  auto it = _connections.find(id);

  if (it != _connections.end())
    return it->second;

  return nullptr;
}


Node*
FlowScene::
getNode(QUuid id) const
{
  auto it = _flowItems.find(id);

  if (it != _flowItems.end())
    return it->second;

  return nullptr;
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
