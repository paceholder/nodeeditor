#include "FlowScene.hpp"

#include <iostream>

#include <QtWidgets/QGraphicsSceneMoveEvent>

#include "Node.hpp"
#include "NodeGraphicsObject.hpp"

#include "ConnectionGraphicsObject.hpp"

#include "FlowItemInterface.hpp"
#include "FlowGraphicsView.hpp"
#include "DataModelRegistry.hpp"

std::shared_ptr<Connection>
FlowScene::
createConnection(PortType connectedPort,
                 std::shared_ptr<Node> node,
                 PortIndex portIndex)
{
  auto connection = std::make_shared<Connection>(connectedPort, node, portIndex);

  auto cgo = std::make_unique<ConnectionGraphicsObject>(*this, connection);

  // after this function connection points are set to node port
  connection->setGraphicsObject(std::move(cgo));

  //connection->getConnectionGraphicsObject()->grabMouse();

  _connections[connection->id()] = connection;

  return connection;
}


void
FlowScene::
deleteConnection(std::shared_ptr<Connection> connection)
{
  _connections.erase(connection->id());
}


std::shared_ptr<Node>
FlowScene::
createNode(std::unique_ptr<NodeDataModel> && dataModel)
{
  auto node = std::make_shared<Node>(std::move(dataModel));

  auto ngo = std::make_unique<NodeGraphicsObject>(*this, node);

  node->setGraphicsObject(std::move(ngo));

  _nodes[node->id()] = node;

  return node;
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
locateNodeAt(QPointF scenePoint, FlowScene &scene, QTransform viewTransform)
{
  // items under cursor
  QList<QGraphicsItem*> items =
    scene.items(scenePoint,
                Qt::IntersectsItemShape,
                Qt::DescendingOrder,
                viewTransform);

  //// items convertable to NodeGraphicsObject
  std::vector<QGraphicsItem*> filteredItems;

  std::copy_if(items.begin(),
               items.end(),
               std::back_inserter(filteredItems),
               [] (QGraphicsItem * item)
               {
                 return (dynamic_cast<NodeGraphicsObject*>(item) != nullptr);
               });

  std::shared_ptr<Node> resultNode;

  if (!filteredItems.empty())
  {
    QGraphicsItem* graphicsItem = filteredItems.front();
    auto ngo = dynamic_cast<NodeGraphicsObject*>(graphicsItem);

    resultNode = ngo->node().lock();
  }

  return resultNode;
}
