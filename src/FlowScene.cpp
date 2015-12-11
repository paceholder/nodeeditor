#include "FlowScene.hpp"

#include <iostream>

#include <QtWidgets/QGraphicsSceneMoveEvent>

#include "Node.hpp"
#include "FlowItemInterface.hpp"
#include "FlowGraphicsView.hpp"


FlowScene* FlowScene::_instance = nullptr;

FlowScene&
FlowScene::
instance()
{
  static FlowScene flowScene;

  return flowScene;
}


Node*
FlowScene::
locateNodeAt(QGraphicsSceneMouseEvent * event)
{
  auto view = static_cast<FlowGraphicsView*>(event->widget());

  auto& scene = FlowScene::instance();

  // items under cursor
  QList<QGraphicsItem*> items = scene.items(event->scenePos(),
                                            Qt::IntersectsItemShape,
                                            Qt::DescendingOrder,
                                            view->transform());

  std::vector<QGraphicsItem*> filteredItems;

  std::copy_if(items.begin(),
               items.end(),
               std::back_inserter(filteredItems),
               [](QGraphicsItem * item)
               {
                 return dynamic_cast<Node*>(item);
               });

  if (filteredItems.empty())
    return nullptr;

  Node* flowItem = dynamic_cast<Node*>(filteredItems.front());

  return flowItem;
}

//QUuid
//FlowScene::
//createConnection(std::pair<QUuid, int> address,
                 //EndType draggingEnd)
//{
  //Connection* connection = new Connection(address, draggingEnd);

  //// add to map
  //_connections[connection->id()] = connection;

  //return connection->id();
//}


Connection*
FlowScene::
createConnection()
{
  auto* connection = new Connection();

  _connections[connection->id()] = connection;

  return connection;
}


QUuid
FlowScene::
createNode()
{

  for (auto i : {1, 2, 3, 4, 5, 6, 7, 8})
  //for (auto i : {1, 2, 3})
  {
    (void)i;

    Node* flowItem = new Node();

    this->addItem(flowItem);

    _flowItems[flowItem->id()] = flowItem;

    flowItem->initializeNode();

    flowItem->moveBy(std::rand() % 500, std::rand() % 500);
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
clearDraggingConnection()
{
  _draggingConnectionID = QUuid();
  _dragging = EndType::NONE;
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
  _dragging(EndType::NONE)
{
  //
}
