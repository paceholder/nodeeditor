#include "FlowScene.hpp"

#include <iostream>

#include <QtWidgets/QGraphicsSceneMoveEvent>

#include "Node.hpp"
#include "NodeGraphicsObject.hpp"

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
                 return (dynamic_cast<NodeGraphicsObject*>(item) != nullptr);
               });

  if (filteredItems.empty())
    return nullptr;

  return dynamic_cast<NodeGraphicsObject*>(filteredItems.front())->node();
}


Connection*
FlowScene::
createConnection()
{
  auto* connection = new Connection();

  _connections[connection->id()] = connection;

  return connection;
}


void
FlowScene::
deleteConnection(Connection* c)
{
  _connections.erase(c->id());

  delete c;
}


QUuid
FlowScene::
createNode()
{
  for (auto i : {1, 2, 3, 4, 5, 6, 7, 8})
  //for (auto i : {1, 2, 3, 4})
  //for (auto i : {1})
  {
    (void)i;

    Node* node = new Node();

    _nodes[node->id()] = node;
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
  auto it = _nodes.find(id);

  if (it != _nodes.end())
    return it->second;

  return nullptr;
}


FlowScene::
FlowScene()
{
  //
}


FlowScene::
~FlowScene()
{
  for (auto &c : _connections)
  {
    delete c.second;
  }

  for (auto &n : _nodes)
  {
    delete n.second;
  }
}
