#include "FlowScene.hpp"

#include <iostream>
#include <stdexcept>

#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtWidgets/QFileDialog>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtCore/QFile>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>

#include <QDebug>

#include "Node.hpp"
#include "NodeGraphicsObject.hpp"

#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"

#include "FlowItemInterface.hpp"
#include "FlowView.hpp"
#include "DataModelRegistry.hpp"

using QtNodes::FlowScene;
using QtNodes::Node;
using QtNodes::NodeGraphicsObject;
using QtNodes::Connection;
using QtNodes::DataModelRegistry;
using QtNodes::NodeDataModel;
//using QtNodes::Properties;
using QtNodes::PortType;
using QtNodes::PortIndex;

FlowScene::
FlowScene(std::shared_ptr<DataModelRegistry> registry)
  : _registry(registry)
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

std::shared_ptr<Connection>
FlowScene::
createConnection(PortType connectedPort,
                 Node& node,
                 PortIndex portIndex)
{
  auto connection = std::make_shared<Connection>(connectedPort, node, portIndex);

  auto cgo = std::make_unique<ConnectionGraphicsObject>(*this, *connection);

  // after this function connection points are set to node port
  connection->setGraphicsObject(std::move(cgo));

  _connections[connection->id()] = connection;

  connectionCreated(*connection);
  return connection;
}


std::shared_ptr<Connection>
FlowScene::
createConnection(Node& nodeIn,
                 PortIndex portIndexIn,
                 Node& nodeOut,
                 PortIndex portIndexOut)
{

  auto connection =
    std::make_shared<Connection>(nodeIn,
                                 portIndexIn,
                                 nodeOut,
                                 portIndexOut);

  auto cgo = std::make_unique<ConnectionGraphicsObject>(*this, *connection);

  nodeIn.nodeState().setConnection(PortType::In, portIndexIn, *connection);
  nodeOut.nodeState().setConnection(PortType::Out, portIndexOut, *connection);

  // after this function connection points are set to node port
  connection->setGraphicsObject(std::move(cgo));

  // trigger data propagation
  nodeOut.onDataUpdated(portIndexOut);

  _connections[connection->id()] = connection;

  connectionCreated(*connection);

  return connection;
}


std::shared_ptr<Connection>
FlowScene::
restoreConnection(QJsonObject const &connectionJson)
{
  QUuid nodeInId  = QUuid(connectionJson["in_id"].toString());
  QUuid nodeOutId = QUuid(connectionJson["out_id"].toString());

  PortIndex portIndexIn  = connectionJson["in_index"].toInt();
  PortIndex portIndexOut = connectionJson["out_index"].toInt();

  auto nodeIn  = _nodes[nodeInId].get();
  auto nodeOut = _nodes[nodeOutId].get();

  return createConnection(*nodeIn, portIndexIn, *nodeOut, portIndexOut);
}


void
FlowScene::
deleteConnection(Connection& connection)
{
  connectionDeleted(connection);
  connection.removeFromNodes();
  _connections.erase(connection.id());
}


Node&
FlowScene::
createNode(std::unique_ptr<NodeDataModel> && dataModel)
{
  auto node = std::make_unique<Node>(std::move(dataModel));
  auto ngo  = std::make_unique<NodeGraphicsObject>(*this, *node);

  node->setGraphicsObject(std::move(ngo));

  auto nodePtr = node.get();
  _nodes[node->id()] = std::move(node);

  nodeCreated(*nodePtr);
  return *nodePtr;
}


Node&
FlowScene::
restoreNode(QJsonObject const& nodeJson)
{
  QString modelName = nodeJson["model"].toObject()["name"].toString();

  auto dataModel = registry().create(modelName);

  if (!dataModel)
    throw std::logic_error(std::string("No registered model with name ") +
                           modelName.toLocal8Bit().data());

  auto node = std::make_unique<Node>(std::move(dataModel));
  auto ngo  = std::make_unique<NodeGraphicsObject>(*this, *node);
  node->setGraphicsObject(std::move(ngo));

  node->restore(nodeJson);

  auto nodePtr = node.get();
  _nodes[node->id()] = std::move(node);

  nodeCreated(*nodePtr);
  return *nodePtr;
}


void
FlowScene::
removeNode(Node& node)
{
  auto deleteConnections = [&node, this] (PortType portType)
  {
    auto nodeState = node.nodeState();
    auto const & nodeEntries = nodeState.getEntries(portType);

    for (auto &connections : nodeEntries)
    {
      for (auto const &pair : connections)
        deleteConnection(*pair.second);
    }
  };

  deleteConnections(PortType::In);
  deleteConnections(PortType::Out);

  // call signal
  nodeDeleted(node);

  _nodes.erase(node.id());
}


DataModelRegistry&
FlowScene::
registry() const
{
  return *_registry;
}


void
FlowScene::
setRegistry(std::shared_ptr<DataModelRegistry> registry)
{
  _registry = registry;
}


void
FlowScene::
iterateOverNodes(std::function<void(Node*)> visitor)
{
  for (const auto& _node : _nodes)
  {
    visitor(_node.second.get());
  }
}


std::unordered_map<QUuid, std::unique_ptr<Node> > const &
FlowScene::
nodes() const
{
  return _nodes;
}


std::unordered_map<QUuid, std::shared_ptr<Connection> > const &
FlowScene::
connections() const
{
  return _connections;
}


//------------------------------------------------------------------------------

void
FlowScene::
save() const
{
  QJsonObject sceneJson;

  QJsonArray nodesJsonArray;

  for (auto const & pair : _nodes)
  {
    auto const &node = pair.second;

    nodesJsonArray.append(node->save());
  }

  sceneJson["nodes"] = nodesJsonArray;

  QJsonArray connectionJsonArray;
  for (auto const & pair : _connections)
  {
    auto const &connection = pair.second;

    QJsonObject connectionJson = connection->save();

    if (!connectionJson.isEmpty())
      connectionJsonArray.append(connectionJson);
  }

  sceneJson["connections"] = connectionJsonArray;

  QString fileName =
    QFileDialog::getSaveFileName(nullptr,
                                 tr("Open Flow Scene"),
                                 QDir::homePath(),
                                 tr("Flow Scene Files (*.flow)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith("flow", Qt::CaseInsensitive))
      fileName += ".flow";

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
      QJsonDocument document(sceneJson);
      file.write(document.toJson());
    }
  }
}


void
FlowScene::
load()
{
  _connections.clear();
  _nodes.clear();

  //-------------

  QString fileName =
    QFileDialog::getOpenFileName(nullptr,
                                 tr("Open Flow Scene"),
                                 QDir::homePath(),
                                 tr("Flow Scene Files (*.flow)"));

  if (!QFileInfo::exists(fileName))
    return;

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly))
    return;

  QByteArray wholeFile = file.readAll();

  QJsonObject const jsonDocument = QJsonDocument::fromJson(wholeFile).object();

  QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();

  for (int i = 0; i < nodesJsonArray.size(); ++i)
  {
    restoreNode(nodesJsonArray[i].toObject());
  }

  QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();

  for (int i = 0; i < connectionJsonArray.size(); ++i)
  {
    restoreConnection(connectionJsonArray[i].toObject());
  }

  //QRectF  r = itemsBoundingRect();
  ////QPointF c = r.center();

  //for (auto &view : views())
  //{
  //qDebug() << "center";

  //view->setSceneRect(r);
  //view->ensureVisible(r);
  //}
}


//------------------------------------------------------------------------------

namespace QtNodes
{

Node*
locateNodeAt(QPointF scenePoint, FlowScene &scene,
             QTransform viewTransform)
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

  Node* resultNode = nullptr;

  if (!filteredItems.empty())
  {
    QGraphicsItem* graphicsItem = filteredItems.front();
    auto ngo = dynamic_cast<NodeGraphicsObject*>(graphicsItem);

    resultNode = &ngo->node();
  }

  return resultNode;
}
}
