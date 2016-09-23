#include "FlowScene.hpp"

#include <iostream>
#include <stdexcept>

#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtWidgets/QFileDialog>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtCore/QFile>

#include <QDebug>

#include "Node.hpp"
#include "NodeGraphicsObject.hpp"

#include "ConnectionGraphicsObject.hpp"

#include "FlowItemInterface.hpp"
#include "FlowView.hpp"
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

  _connections[connection->id()] = connection;

  return connection;
}


std::shared_ptr<Connection>
FlowScene::
restoreConnection(Properties const &p)
{

  QUuid nodeInId;
  QUuid nodeOutId;

  p.get("in_id", &nodeInId);
  p.get("out_id", &nodeOutId);

  PortIndex portIndexIn;
  PortIndex portIndexOut;

  p.get("in_index", &portIndexIn);
  p.get("out_index", &portIndexOut);

  auto nodeIn  = _nodes[nodeInId];
  auto nodeOut = _nodes[nodeOutId];

  auto connection =
    std::make_shared<Connection>(nodeIn,
                                 portIndexIn,
                                 nodeOut,
                                 portIndexOut);

  auto cgo = std::make_unique<ConnectionGraphicsObject>(*this, connection);

  nodeIn->nodeState().setConnection(PortType::In, portIndexIn, connection);
  nodeOut->nodeState().setConnection(PortType::Out, portIndexOut, connection);

  // trigger data propagation
  nodeOut->onDataUpdated(portIndexOut);

  // after this function connection points are set to node port
  connection->setGraphicsObject(std::move(cgo));

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
  auto ngo  = std::make_unique<NodeGraphicsObject>(*this, node);

  node->setGraphicsObject(std::move(ngo));

  _nodes[node->id()] = node;

  return node;
}


std::shared_ptr<Node>
FlowScene::
restoreNode(Properties const &p)
{
  QString modelName;

  p.get("model_name", &modelName);

  auto const &models = DataModelRegistry::registeredModels();
  auto it = models.find(modelName);

  if (it == models.end())
    throw std::logic_error(std::string("No registered model with name ") +
                           modelName.toLocal8Bit().data());

  auto dataModel = it->second->create();
  auto node      = std::make_shared<Node>(std::move(dataModel));
  auto ngo       = std::make_unique<NodeGraphicsObject>(*this, node);
  node->setGraphicsObject(std::move(ngo));

  node->restore(p);

  _nodes[node->id()] = node;
  return node;
}


void
FlowScene::
removeNode(QGraphicsItem* item)
{
  auto ngo = dynamic_cast<NodeGraphicsObject*>(item);

  std::shared_ptr<Node> const& node = ngo->node().lock();

  auto deleteConnections = [&node, this] (PortType portType)
  {
    auto nodeState = node->nodeState();
    auto const & nodeEntries = nodeState.getEntries(portType);

    for (std::weak_ptr<Connection> conn : nodeEntries)
    {
      if (auto c = conn.lock())
        this->deleteConnection(c);
    }
  };

  deleteConnections(PortType::In);
  deleteConnections(PortType::Out);

  _nodes.erase(node->id());
}


void
FlowScene::
save() const
{
  QByteArray byteArray;
  QBuffer    writeBuffer(&byteArray);

  writeBuffer.open(QIODevice::WriteOnly);
  QDataStream out(&writeBuffer);

  out << static_cast<quint64>(_nodes.size());

  for (auto const & pair : _nodes)
  {
    auto const &node = pair.second;

    Properties p;

    node->save(p);

    QVariantMap const &m = p.values();

    out << m;
  }

  out << static_cast<quint64>(_connections.size());

  for (auto const & pair : _connections)
  {
    auto const &connection = pair.second;

    Properties p;

    connection->save(p);

    QVariantMap const &m = p.values();

    out << m;
  }

  //qDebug() << byteArray;

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
    file.open(QIODevice::WriteOnly);
    file.write(byteArray);
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

  QDataStream in(&file);

  qint64 nNodes;
  in >> nNodes;

  for (unsigned int i = 0; i < nNodes; ++i)
  {
    Properties p;
    auto &values = p.values();
    in >> values;

    restoreNode(p);
  }

  qint64 nConnections;
  in >> nConnections;

  for (unsigned int i = 0; i < nConnections; ++i)
  {
    Properties p;
    auto &values = p.values();
    in >> values;

    restoreConnection(p);
  }
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
