#include "FlowScene.hpp"

#include <iostream>

#include <QtWidgets/QGraphicsSceneMoveEvent>

#include "Node.hpp"
#include "NodeGraphicsObject.hpp"

#include "FlowItemInterface.hpp"
#include "FlowGraphicsView.hpp"
#include "DataModelRegistry.hpp"

FlowScene&
FlowScene::
instance()
{
  static FlowScene flowScene;

  return flowScene;
}


std::shared_ptr<Connection>
FlowScene::
createConnection()
{
  auto connection = std::make_shared<Connection>();

  _connections[connection->id()] = connection;

  return connection;
}


void
FlowScene::
deleteConnection(QUuid const & id)
{
  _connections.erase(id);
}


QUuid
FlowScene::
createNodes()
{
  //for (auto i : {1, 2, 3, 4})
  ////for (auto i : {1})
  //{
    //(void)i; // unused variable

    //auto node = std::make_shared<Node>();

    //_nodes[node->id()] = node;
  //}

  return QUuid();
}


std::shared_ptr<Node>
FlowScene::
createNode(std::unique_ptr<NodeDataModel> &&dataModel)
{
  std::cout << "Create Node" << std::endl;

  auto node = std::make_shared<Node>(std::move(dataModel));

  _nodes[node->id()] = node;

  return node;
}


std::shared_ptr<Connection>
FlowScene::
getConnection(QUuid id) const
{
  auto it = _connections.find(id);

  Q_ASSERT_X(it != _connections.end(),
             "getConnection()",
             "Connection with given ID does not exist");

  return it->second;
}


std::shared_ptr<Node>
FlowScene::
getNode(QUuid id) const
{
  auto it = _nodes.find(id);

  if (it == _nodes.end())
    return std::shared_ptr<Node>();

  return it->second;
}


FlowScene::
FlowScene()
{
  setItemIndexMethod(QGraphicsScene::NoIndex);
}


FlowScene::
~FlowScene()
{
  _connections.clear();
  _nodes.clear();
}


//------------------------------------------------------------------------------

std::shared_ptr<Node>
locateNodeAt(QGraphicsSceneMouseEvent * event)
{
  auto view = static_cast<FlowGraphicsView*>(event->widget());

  auto& scene = FlowScene::instance();

  // items under cursor
  QList<QGraphicsItem*> items =
    scene.items(event->scenePos(),
                Qt::IntersectsItemShape,
                Qt::DescendingOrder,
                view->transform());

  // items convertable to NodeGraphicsObject
  std::vector<QGraphicsItem*> filteredItems;

  std::copy_if(items.begin(),
               items.end(),
               std::back_inserter(filteredItems),
               [](QGraphicsItem * item)
               {
                 return (dynamic_cast<NodeGraphicsObject*>(item) != nullptr);
               });

  std::shared_ptr<Node> resultNode;

  if (!filteredItems.empty())
  {
    QGraphicsItem* graphicsItem = filteredItems.front();
    auto ngo = dynamic_cast<NodeGraphicsObject*>(graphicsItem);

    resultNode = scene.getNode(ngo->node().id());
  }

  return resultNode;
}
