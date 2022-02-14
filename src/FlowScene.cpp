#include "FlowScene.hpp"

#include <stdexcept>
#include <utility>
#include <unordered_set>

#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtWidgets/QFileDialog>
#include <QtCore/QByteArray>
#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QString>

#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonArray>
#include <QtCore/QtGlobal>
#include <QtCore/QDebug>
#include <QDir>

#include "Node.hpp"
#include "NodeGraphicsObject.hpp"

#include "ConnectionGraphicsObject.hpp"

#include "GroupGraphicsObject.hpp"

#include "Connection.hpp"

#include "FlowView.hpp"
#include "DataModelRegistry.hpp"

using QtNodes::FlowScene;
using QtNodes::Node;
using QtNodes::NodeGraphicsObject;
using QtNodes::Connection;
using QtNodes::DataModelRegistry;
using QtNodes::NodeDataModel;
using QtNodes::PortType;
using QtNodes::PortIndex;
using QtNodes::TypeConverter;
using QtNodes::NodeGroup;
using QtNodes::GroupGraphicsObject;

FlowScene::
FlowScene(std::shared_ptr<DataModelRegistry> registry,
          QObject * parent)
  : QGraphicsScene(parent)
  , _registry(std::move(registry))
{
  setItemIndexMethod(QGraphicsScene::NoIndex);

  // Defines the sceneRect as the maximum rectangle achievable using int ranges. Theoretically
  // a QGraphicsScene could have infinite area, but the area that a scroll bar can reach is in
  // the range (INT_MIN, INT_MAX).
  QRect maximumRect{QPoint(std::numeric_limits<int>::lowest()/2,
                           std::numeric_limits<int>::lowest()/2),
                    QSize(std::numeric_limits<int>::max(),
                          std::numeric_limits<int>::max())};
  setSceneRect(maximumRect);

  // This connection should come first
  connect(this, &FlowScene::connectionCreated, this, &FlowScene::setupConnectionSignals);
  connect(this, &FlowScene::connectionCreated, this, &FlowScene::sendConnectionCreatedToNodes);
  connect(this, &FlowScene::connectionDeleted, this, &FlowScene::sendConnectionDeletedToNodes);
}

FlowScene::
FlowScene(QObject * parent)
  : FlowScene(std::make_shared<DataModelRegistry>(),
              parent)
{}


FlowScene::
~FlowScene()
{
  clearScene();
}


//------------------------------------------------------------------------------

std::shared_ptr<Connection>
FlowScene::
createConnection(PortType connectedPort,
                 Node& node,
                 PortIndex portIndex)
{
  auto connection = std::make_shared<Connection>(connectedPort, node, portIndex);

  auto cgo = detail::make_unique<ConnectionGraphicsObject>(*this, *connection);

  // after this function connection points are set to node port
  connection->setGraphicsObject(std::move(cgo));

  _connections[connection->id()] = connection;

  // Note: this connection isn't truly created yet. It's only partially created.
  // Thus, don't send the connectionCreated(...) signal.

  connect(connection.get(),
          &Connection::connectionCompleted,
          this,
          [this](Connection const& c)
  {
    connectionCreated(c);
  });

  return connection;
}


std::shared_ptr<Connection>
FlowScene::
createConnection(Node& nodeIn,
                 PortIndex portIndexIn,
                 Node& nodeOut,
                 PortIndex portIndexOut,
                 TypeConverter const &converter)
{
  auto connection =
    std::make_shared<Connection>(nodeIn,
                                 portIndexIn,
                                 nodeOut,
                                 portIndexOut,
                                 converter);

  auto cgo = detail::make_unique<ConnectionGraphicsObject>(*this, *connection);

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
  return loadConnectionToMap(connectionJson, _nodes, _connections);
}

std::shared_ptr<Connection>
FlowScene::
loadConnectionToMap(const QJsonObject& connectionJson,
                    const std::unordered_map<QUuid, std::unique_ptr<Node>>& nodesMap,
                    std::unordered_map<QUuid, std::shared_ptr<Connection> >& connectionsMap,
                    const std::unordered_map<QUuid, QUuid>& IDMap)
{
  QUuid nodeInId  = QUuid(connectionJson["in_id"].toString());
  QUuid nodeOutId = QUuid(connectionJson["out_id"].toString());

  if (!IDMap.empty())
  {
    nodeInId = IDMap.at(nodeInId);
    nodeOutId = IDMap.at(nodeOutId);
  }

  PortIndex portIndexIn  = connectionJson["in_index"].toInt();
  PortIndex portIndexOut = connectionJson["out_index"].toInt();

  auto nodeIn  = nodesMap.at(nodeInId).get();
  auto nodeOut = nodesMap.at(nodeOutId).get();

  const TypeConverter& converter = getConverter(connectionJson);

  auto connection =
    std::make_shared<Connection>(*nodeIn,
                                 portIndexIn,
                                 *nodeOut,
                                 portIndexOut,
                                 converter);
  auto cgo = detail::make_unique<ConnectionGraphicsObject>(*this, *connection);

  nodeIn->nodeState().setConnection(PortType::In, portIndexIn, *connection);
  nodeOut->nodeState().setConnection(PortType::Out, portIndexOut, *connection);

  // after this function connection points are set to node port
  connection->setGraphicsObject(std::move(cgo));

  // trigger data propagation
  nodeOut->onDataUpdated(portIndexOut);

  connectionsMap[connection->id()] = connection;

  connectionCreated(*connection);
  return connection;
}

void
FlowScene::
deleteConnection(Connection const& connection)
{
  auto it = _connections.find(connection.id());
  if (it != _connections.end())
  {
    connection.removeFromNodes();
    _connections.erase(it);
  }
}

TypeConverter
FlowScene::
getConverter(const QJsonObject& connectionJson)
{
  QJsonValue converterVal = connectionJson["converter"];

  if (!converterVal.isUndefined())
  {
    QJsonObject converterJson = converterVal.toObject();

    NodeDataType inType { converterJson["in"].toObject()["id"].toString(),
                          converterJson["in"].toObject()["name"].toString() };

    NodeDataType outType { converterJson["out"].toObject()["id"].toString(),
                           converterJson["out"].toObject()["name"].toString() };

    auto converter  =
      registry().getTypeConverter(outType, inType);

    if (converter)
      return converter;
  }
  return TypeConverter{};
}


Node&
FlowScene::
createNode(std::unique_ptr<NodeDataModel> && dataModel)
{
  auto node = detail::make_unique<Node>(std::move(dataModel));
  auto ngo  = detail::make_unique<NodeGraphicsObject>(*this, *node);

  node->setGraphicsObject(std::move(ngo));

  auto nodePtr = node.get();
  _nodes[node->id()] = std::move(node);

  nodeCreated(*nodePtr);
  return *nodePtr;
}


Node&
FlowScene::
restoreNode(QJsonObject const& nodeJson, bool keep_id)
{
  return loadNodeToMap(nodeJson, _nodes, keep_id);
}


Node&
FlowScene::
loadNodeToMap(const QJsonObject& nodeJson,
              std::unordered_map<QUuid, std::unique_ptr<Node>>& map,
              bool keep_id)
{
  QString modelName = nodeJson["model"].toObject()["name"].toString();

  auto dataModel = registry().create(modelName);

  if (!dataModel)
    throw std::logic_error(std::string("No registered model with name ") +
                           modelName.toLocal8Bit().data());

  auto node = detail::make_unique<Node>(std::move(dataModel));
  auto ngo  = detail::make_unique<NodeGraphicsObject>(*this, *node);
  node->setGraphicsObject(std::move(ngo));

  if(keep_id) node->retrieveID(nodeJson);
  auto nodeID = node->id();
  map[nodeID] = std::move(node);
  auto nodePtr = map[nodeID].get();
  nodeCreated(*nodePtr);
  nodePtr->restore(nodeJson);

  nodePlaced(*nodePtr);
  return *nodePtr;
}


void
FlowScene::
removeNode(Node& node)
{
  // call signal
  nodeDeleted(node);

  for(auto portType:
      {
        PortType::In,PortType::Out
      })
  {
    auto nodeState = node.nodeState();
    auto const & nodeEntries = nodeState.getEntries(portType);

    for (auto &connections : nodeEntries)
    {
      for (auto const &pair : connections)
        deleteConnection(*pair.second);
    }
  }

  if (!node.nodeGroup().expired())
  {
    removeNodeFromGroup(node.id());
  }

  _nodes.erase(node.id());
}

std::weak_ptr<NodeGroup>
FlowScene::
createGroup(std::vector<Node*>& nodes, QString groupName)
{
  if (nodes.empty())
    return std::weak_ptr<NodeGroup>();

  // remove nodes from their previous group
  for (auto* node : nodes)
  {
    if (!node->nodeGroup().expired())
      removeNodeFromGroup(node->id());
  }

  if (groupName == QStringLiteral(""))
  {
    groupName = "Group " + QString::number(NodeGroup::groupCount());
  }
  auto group = std::make_shared<NodeGroup>(nodes, QUuid::createUuid(), groupName, this);
  auto ggo   = std::make_unique<GroupGraphicsObject>(*this, *group);

  group->setGraphicsObject(std::move(ggo));

  for (auto& nodePtr : nodes)
  {
    auto node = _nodes[nodePtr->id()].get();
    node->setNodeGroup(group);
  }

  std::weak_ptr<NodeGroup> groupWeakPtr = group;

  _groups[group->id()] = std::move(group);

  return groupWeakPtr;
}

std::weak_ptr<QtNodes::NodeGroup>
FlowScene::
createGroupFromSelection(QString groupName)
{
  auto nodes = selectedNodes();
  return createGroup(nodes, groupName);
}

std::vector<std::shared_ptr<Connection> >
FlowScene::
connectionsWithinGroup(const QUuid& groupID)
{
  std::vector<std::shared_ptr<Connection>> ret{};

  for (auto const & connection : _connections)
  {
    auto node1 = connection.second->getNode(PortType::In)->nodeGroup().lock();
    auto node2 = connection.second->getNode(PortType::Out)->nodeGroup().lock();
    if ( node1 && node2 )
    {
      if ((node1->id() == node2->id()) && (node1->id() == groupID))
      {
        ret.push_back(connection.second);
      }
    }
  }

  return ret;
}

std::pair<std::weak_ptr<NodeGroup>,std::unordered_map<QUuid,QUuid> >
FlowScene::
restoreGroup(QJsonObject const& groupJson)
{
  // since the new nodes will have the same IDs as in the file and the connections
  // need these old IDs to be restored, we must create new IDs and map them to the
  // old ones so the connections are properly restored
  std::unordered_map<QUuid, QUuid> IDsMap{};

  std::vector<Node*> group_children{};

  QJsonArray nodesJson = groupJson["nodes"].toArray();
  for (const QJsonValueRef nodeJson : nodesJson)
  {
    auto oldID = QUuid(nodeJson.toObject()["id"].toString());
    auto& nodeRef = loadNodeToMap(nodeJson.toObject(), _nodes, false);

    auto newID = nodeRef.id();

    IDsMap.insert(std::make_pair(oldID, newID));
    group_children.push_back(&nodeRef);
  }

  QJsonArray connectionJsonArray = groupJson["connections"].toArray();
  for (auto connection : connectionJsonArray)
  {
    loadConnectionToMap(connection.toObject(), _nodes, _connections, IDsMap);
  }

  return std::make_pair(
           createGroup(group_children, groupJson["name"].toString()),
           IDsMap);
}

void
FlowScene::
removeGroup(const QUuid& groupID)
{
  auto group = _groups.at(groupID);
  group->groupGraphicsObject().lock(false);
  while (!group->childNodes().empty())
  {
    removeNode(*group->childNodes().back());
  }
  _groups.erase(group->id());
}

void
FlowScene::
addNodeToGroup(const QUuid& nodeID,
               const QUuid& groupID)
{
  auto group = _groups.at(groupID);
  auto node = _nodes.at(nodeID).get();
  group->addNode(node);
  node->setNodeGroup(group);
}

void
FlowScene::
removeNodeFromGroup(const QUuid& nodeID)
{
  auto nodeIt = _nodes.at(nodeID).get();
  if (auto group = nodeIt->nodeGroup().lock(); group)
  {
    group->removeNode(nodeIt);
    if (group->empty())
    {
      removeGroup(group->id());
    }
  }
  nodeIt->unsetNodeGroup();
  nodeIt->nodeGraphicsObject().lock(false);
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
  _registry = std::move(registry);
}


void
FlowScene::
iterateOverNodes(std::function<void(Node*)> const & visitor)
{
  for (const auto& _node : _nodes)
  {
    visitor(_node.second.get());
  }
}


void
FlowScene::
iterateOverNodeData(std::function<void(NodeDataModel*)> const & visitor)
{
  for (const auto& _node : _nodes)
  {
    visitor(_node.second->nodeDataModel());
  }
}


void
FlowScene::
iterateOverNodeDataDependentOrder(std::function<void(NodeDataModel*)> const & visitor)
{
  std::set<QUuid> visitedNodesSet;

  //A leaf node is a node with no input ports, or all possible input ports empty
  auto isNodeLeaf =
    [](Node const &node, NodeDataModel const &model)
  {
    for (unsigned int i = 0; i < model.nPorts(PortType::In); ++i)
    {
      auto connections = node.nodeState().connections(PortType::In, i);
      if (!connections.empty())
      {
        return false;
      }
    }

    return true;
  };

  //Iterate over "leaf" nodes
  for (auto const &_node : _nodes)
  {
    auto const &node = _node.second;
    auto model       = node->nodeDataModel();

    if (isNodeLeaf(*node, *model))
    {
      visitor(model);
      visitedNodesSet.insert(node->id());
    }
  }

  auto areNodeInputsVisitedBefore =
    [&](Node const &node, NodeDataModel const &model)
  {
    for (size_t i = 0; i < model.nPorts(PortType::In); ++i)
    {
      auto connections = node.nodeState().connections(PortType::In, i);

      for (auto& conn : connections)
      {
        if (visitedNodesSet.find(conn.second->getNode(PortType::Out)->id()) == visitedNodesSet.end())
        {
          return false;
        }
      }
    }

    return true;
  };

  //Iterate over dependent nodes
  while (_nodes.size() != visitedNodesSet.size())
  {
    for (auto const &_node : _nodes)
    {
      auto const &node = _node.second;
      if (visitedNodesSet.find(node->id()) != visitedNodesSet.end())
        continue;

      auto model = node->nodeDataModel();

      if (areNodeInputsVisitedBefore(*node, *model))
      {
        visitor(model);
        visitedNodesSet.insert(node->id());
      }
    }
  }
}


QPointF
FlowScene::
getNodePosition(const Node& node) const
{
  return node.nodeGraphicsObject().pos();
}


void
FlowScene::
setNodePosition(Node& node, const QPointF& pos) const
{
  node.nodeGraphicsObject().setPos(pos);
  node.nodeGraphicsObject().moveConnections();
}


QSizeF
FlowScene::
getNodeSize(const Node& node) const
{
  return QSizeF(node.nodeGeometry().width(), node.nodeGeometry().height());
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

std::unordered_map<QUuid, std::shared_ptr<NodeGroup> > const &
FlowScene::
groups() const
{
  return _groups;
}


std::vector<Node*>
FlowScene::
allNodes() const
{
  std::vector<Node*> nodes;

  std::transform(_nodes.begin(),
                 _nodes.end(),
                 std::back_inserter(nodes),
                 [](std::pair<QUuid const, std::unique_ptr<Node>> const & p)
  {
    return p.second.get();
  });

  return nodes;
}


std::vector<Node*>
FlowScene::
selectedNodes() const
{
  QList<QGraphicsItem*> graphicsItems = selectedItems();

  std::vector<Node*> ret;
  ret.reserve(graphicsItems.size());

  std::unordered_set<QUuid> addedIDs{};

  for (QGraphicsItem* item : graphicsItems)
  {
    if (auto* ngo = qgraphicsitem_cast<NodeGraphicsObject*>(item); ngo)
    {
      Node* node = &ngo->node();
      ret.push_back(node);
      addedIDs.insert(node->id());
    }
    else if (auto* ggo = qgraphicsitem_cast<GroupGraphicsObject*>(item); ggo)
    {
      for (auto* node : ggo->group().childNodes())
      {
        if (addedIDs.insert(node->id()).second)
        {
          ret.push_back(node);
        }
      }
    }
  }

  return ret;
}


//------------------------------------------------------------------------------

void
FlowScene::
clearScene()
{
  //Manual node cleanup. Simply clearing the holding datastructures doesn't work, the code crashes when
  // there are both nodes and connections in the scene. (The data propagation internal logic tries to propagate
  // data through already freed connections.)
  while (_connections.size() > 0)
  {
    deleteConnection( *_connections.begin()->second );
  }

  while (_nodes.size() > 0)
  {
    removeNode( *_nodes.begin()->second );
  }
}


void
FlowScene::
save(const QString& fileName) const
{
  QFile file(fileName);
  if (file.open(QIODevice::WriteOnly))
  {
    file.write(saveToMemory());
  }
}


QString
FlowScene::
load()
{
  QString fileName =
    QFileDialog::getOpenFileName(nullptr,
                                 tr("Open Flow Scene"),
                                 QDir::currentPath(),
                                 tr("Flow Scene Files (*.flow)"));

  return this->load(fileName);
}

QString
FlowScene::
load(const QString& fileName) {
  if (!QFileInfo::exists(fileName))
    return QString();

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) 
    return QString();

  QDir d = QFileInfo(fileName).absoluteDir();
  QString absolute=d.absolutePath();
  QDir::setCurrent(absolute);

  clearScene();

  clearScene();

  QByteArray wholeFile = file.readAll();

  loadFromMemory(wholeFile);

  return fileName;
}

QByteArray
FlowScene::
saveToMemory() const
{
  return saveItems(items());
}


void
FlowScene::
loadFromMemory(const QByteArray& data)
{
  loadItems(data, QPointF(), false);
  clearSelection();
}

QByteArray
FlowScene::
saveItems(const QList<QGraphicsItem *> &items) const
{
  QJsonObject itemsJson;
  QJsonArray nodesJsonArray;
  QJsonArray connectionsJsonArray;
  QJsonArray groupsJsonArray;
  std::unordered_set<QUuid> savedNodeIDs{};

  for (auto* item : items)
  {
    {
      if (auto* ggo = qgraphicsitem_cast<GroupGraphicsObject*>(item))
      {
        auto groupJson =  QJsonDocument::fromJson(ggo->group().saveToFile());
        groupsJsonArray.append(groupJson.object());

        auto& groupChildren = ggo->group().childNodes();
        for (const auto& node : groupChildren)
        {
          savedNodeIDs.insert(node->id());
        }
      }
    }
  }
  for (auto* item : items)
  {
    if (auto* ngo = qgraphicsitem_cast<NodeGraphicsObject*>(item))
    {
      if (savedNodeIDs.find(ngo->node().id()) == savedNodeIDs.end())
      {
        savedNodeIDs.insert(ngo->node().id());
        nodesJsonArray.append(ngo->node().save());
      }
    }
  }
  itemsJson["nodes"] = nodesJsonArray;
  itemsJson["groups"] = groupsJsonArray;

  for (auto* item : items)
  {
    if (auto* cgo = qgraphicsitem_cast<ConnectionGraphicsObject*>(item))
    {
      QJsonObject connectionJson = cgo->connection().save();
      auto inID = cgo->connection()._inNode->id();
      auto outID = cgo->connection()._outNode->id();
      if (!connectionJson.isEmpty()
          && savedNodeIDs.count(inID) != 0
          && savedNodeIDs.count(outID) != 0)
      {
        connectionsJsonArray.append(connectionJson);
      }
    }
  }
  itemsJson["connections"] = connectionsJsonArray;

  QJsonDocument document(itemsJson);

  return document.toJson();
}

QByteArray
FlowScene::
saveItems(QGraphicsItem *item) const
{
  QList<QGraphicsItem*> dummyList({item});
  return saveItems(dummyList);
}

void
FlowScene::
loadItems(const QByteArray& data, QPointF pastePos, bool usePastePos)
{
  QJsonObject const jsonDocument = QJsonDocument::fromJson(data).object();

  // maps the stored (old) node UIDs to their new assigned UIDs
  std::unordered_map<QUuid, QUuid> IDMap{};

  QPointF offset;
  bool offsetInitialized{false};
  clearSelection();

  QJsonArray groupsJsonArray = jsonDocument["groups"].toArray();
  for (const auto& group: groupsJsonArray)
  {
    auto [groupWeakPtr, groupIDsMap] = restoreGroup(group.toObject());
    IDMap.merge(groupIDsMap);
    if (auto groupPtr = groupWeakPtr.lock(); groupPtr)
    {
      auto& ggoRef = groupPtr->groupGraphicsObject();
      if (usePastePos && !offsetInitialized)
      {
        offset = pastePos - ggoRef.pos();
        offsetInitialized = true;
      }
      if (usePastePos)
      {
        ggoRef.moveNodes(offset);
      }
      ggoRef.moveConnections();
      ggoRef.setSelected(true);
    }
  }
  QJsonArray nodesJsonArray = jsonDocument["nodes"].toArray();
  for (QJsonValueRef node : nodesJsonArray)
  {
    auto nodeObj = node.toObject();
    auto& nodeRef = restoreNode(nodeObj, false);

    QUuid oldID{nodeObj["id"].toString()};
    QUuid newID{nodeRef.id()};
    IDMap.insert(std::make_pair(oldID, newID));

    auto& ngoRef = nodeRef.nodeGraphicsObject();
    if (usePastePos && !offsetInitialized)
    {
      offset = pastePos - ngoRef.pos();
      offsetInitialized = true;
    }
    if (usePastePos)
    {
      ngoRef.moveBy(offset.x(), offset.y());
    }
    ngoRef.moveConnections();
    ngoRef.setSelected(true);
  }

  QJsonArray connectionJsonArray = jsonDocument["connections"].toArray();
  for (QJsonValueRef connection : connectionJsonArray)
  {
    auto connPtr = loadConnectionToMap(connection.toObject(), _nodes,
                                       _connections, IDMap);
    if (connPtr)
    {
      connPtr->getConnectionGraphicsObject().setSelected(true);
    }
  }
}

bool
FlowScene::
checkCopyableSelection() const
{
  auto selection = selectedItems();
  for (const auto& item : selection)
  {
    if (auto ngo = qgraphicsitem_cast<NodeGraphicsObject*>(item); ngo)
      return true;
    if (auto ggo = qgraphicsitem_cast<GroupGraphicsObject*>(item); ggo)
      return true;
  }
  return false;
}

void
FlowScene::
saveGroupFile(const QUuid& groupID)
{
  QString fileName =
    QFileDialog::getSaveFileName(nullptr,
                                 tr("Save Node Group"),
                                 QDir::homePath(),
                                 tr("Node Group files (*.group)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith("group", Qt::CaseInsensitive))
      fileName += ".group";

    if (auto groupIt = _groups.find(groupID); groupIt != _groups.end())
    {
      QFile file(fileName);
      if (file.open(QIODevice::WriteOnly))
      {
        file.write(groupIt->second->saveToFile());
      }
      else
      {
        qDebug() << "Error saving group file!";
      }
    }
    else
    {
      qDebug() << "Error! Couldn't find group while saving.";
    }
  }
}

std::weak_ptr<NodeGroup>
FlowScene::
loadGroupFile()
{
  QString fileName =
    QFileDialog::getOpenFileName(nullptr,
                                 tr("Open Node Group"),
                                 QDir::currentPath(),
                                 tr("Node Group files (*.group)"));

  if (!QFileInfo::exists(fileName))
    return std::weak_ptr<NodeGroup>();

  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly))
  {
    qDebug() << "Error loading group file!";
  }

  QDir d = QFileInfo(fileName).absoluteDir();
  QString absolute=d.absolutePath();
  QDir::setCurrent(absolute);

  QByteArray wholeFile = file.readAll();

  const QJsonObject fileJson = QJsonDocument::fromJson(wholeFile).object();

  return restoreGroup(fileJson).first;
}

void
FlowScene::
nodePortsChanged(const QUuid& nodeId,
                 const QtNodes::PortType portType,
                 unsigned int nPorts)
{
  auto nodeIt = _nodes.find(nodeId);
  if (nodeIt != _nodes.end())
  {
    auto node = nodeIt->second.get();
    auto previousNPorts = node->nodeState().getEntries(portType).size();

    /** @todo as of now, the nPorts() method of the node isn't changed and
     * must be handled externally by the final node class. This could be
     * changed to improve consistency. */

    if (nPorts > previousNPorts)
    {
      for (unsigned int i = previousNPorts; i < nPorts; i++)
      {
        insertNodePort(nodeId, portType, i);
      }
    }
    else if (nPorts < previousNPorts)
    {
      for (unsigned int i = previousNPorts; i > nPorts; i--)
      {
        eraseNodePort(nodeId, portType, i - 1);
      }
    }
  }
  else
  {
    qDebug() << "Error changing number of ports! Node ID not found.";
  }
}

void
FlowScene::
insertNodePort(const QUuid& nodeId,
               const QtNodes::PortType portType,
               size_t index)
{
  auto nodeIt = _nodes.find(nodeId);
  if (nodeIt != _nodes.end())
  {
    auto node = nodeIt->second.get();
    node->nodeState().insertPort(portType, index);
    node->nodeGraphicsObject().updateGeometry();
  }
  else
  {
    qDebug() << "Error inserting node port! Node ID not found.";
  }
}

void
FlowScene::
eraseNodePort(const QUuid& nodeId,
              const QtNodes::PortType portType,
              size_t index)
{
  auto nodeIt = _nodes.find(nodeId);
  if (nodeIt != _nodes.end())
  {
    auto node = nodeIt->second.get();
    auto nodeEntries = node->nodeState().getEntries(portType);

    if (index < nodeEntries.size())
    {
      auto deletedPort = nodeEntries.at(index);

      std::vector<Connection*> portConnections{};
      portConnections.reserve(deletedPort.size());
      for (const auto& entry : deletedPort)
      {
        portConnections.push_back(entry.second);
      }

      for (auto& connection : portConnections)
      {
        deleteConnection(*connection);
      }

      node->nodeState().erasePort(portType, index);
      node->nodeGraphicsObject().updateGeometry();
    }
    else
    {
      qDebug() << "Error removing node port! Invalid port index.";
    }
  }
  else
  {
    qDebug() << "Error removing node port! Node ID not found.";
  }
}


void
FlowScene::
setupConnectionSignals(Connection const& c)
{
  connect(&c,
          &Connection::connectionMadeIncomplete,
          this,
          &FlowScene::connectionDeleted,
          Qt::UniqueConnection);
}


void
FlowScene::
sendConnectionCreatedToNodes(Connection const& c)
{
  Node* from = c.getNode(PortType::Out);
  Node* to   = c.getNode(PortType::In);

  Q_ASSERT(from != nullptr);
  Q_ASSERT(to != nullptr);

  from->nodeDataModel()->outputConnectionCreated(c);
  to->nodeDataModel()->inputConnectionCreated(c);
}


void
FlowScene::
sendConnectionDeletedToNodes(Connection const& c)
{
  Node* from = c.getNode(PortType::Out);
  Node* to   = c.getNode(PortType::In);

  Q_ASSERT(from != nullptr);
  Q_ASSERT(to != nullptr);

  from->nodeDataModel()->outputConnectionDeleted(c);
  to->nodeDataModel()->inputConnectionDeleted(c);
}


//------------------------------------------------------------------------------
namespace QtNodes
{

Node*
locateNodeAt(QPointF scenePoint, FlowScene &scene,
             QTransform const & viewTransform)
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
