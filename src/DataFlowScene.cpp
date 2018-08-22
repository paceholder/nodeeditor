#include "DataFlowScene.hpp"
#include "Connection.hpp"
#include "Node.hpp"

#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>

using QtNodes::DataFlowScene;
using QtNodes::DataModelRegistry;
using QtNodes::Connection;
using QtNodes::Node;
using QtNodes::ConnectionID;
using QtNodes::NodeIndex;
using QtNodes::NodeValidationState;
using QtNodes::NodePainterDelegate;
using QtNodes::NodeDataType;
using QtNodes::ConnectionPolicy;
using QtNodes::PortIndex;

class DataFlowScene::DataFlowModel : public FlowSceneModel {
public:
  
  DataFlowModel(std::shared_ptr<DataModelRegistry> reg);

  // FlowSceneModel read interface
  QStringList modelRegistry() const override;
  QString nodeTypeCatergory(QString const& name) const override;
  bool getTypeConvertable(TypeConverterId const& id ) const override;
  QList<QUuid> nodeUUids() const override;
  NodeIndex nodeIndex(const QUuid& ID) const override;
  QString nodeTypeIdentifier(NodeIndex const& index) const override;
  QString nodeCaption(NodeIndex const& index) const override;
  QPointF nodeLocation(NodeIndex const& index) const override;
  QWidget* nodeWidget(NodeIndex const& index) const override;
  bool nodeResizable(NodeIndex const& index) const override;
  NodeValidationState nodeValidationState(NodeIndex const& index) const override;
  QString nodeValidationMessage(NodeIndex const& index) const override;
  NodePainterDelegate* nodePainterDelegate(NodeIndex const& index) const override;
  unsigned int nodePortCount(NodeIndex const& index, PortType portType) const override;
  QString nodePortCaption(NodeIndex const& index, PortIndex pIndex, PortType portType) const override;
  NodeDataType nodePortDataType(NodeIndex const& index, PortIndex pIndex, PortType portType) const override;
  ConnectionPolicy nodePortConnectionPolicy(NodeIndex const& index, PortIndex pIndex, PortType portType) const override;
  std::vector<std::pair<NodeIndex, PortIndex>> nodePortConnections(NodeIndex const& index, PortIndex id, PortType portType) const override;

  // FlowSceneModel write interface
  bool removeConnection(NodeIndex const& leftNode, PortIndex leftPortID, NodeIndex const& rightNode, PortIndex rightPortID) override;
  bool addConnection(NodeIndex const& leftNode, PortIndex leftPortID, NodeIndex const& rightNode, PortIndex rightPortID) override;
  bool removeNode(NodeIndex const& index) override;
  QUuid addNode(QString const& typeID, QPointF const& location) override;
  bool moveNode(NodeIndex const& index, QPointF newLocation) override;

  // convenience functions
  QUuid addNode(QString const& typeID, QPointF const& location, QUuid const& uuid);
  QUuid addNode(std::unique_ptr<NodeDataModel>&& model, QPointF const& location, QUuid const& uuid = QUuid::createUuid());
  ConnectionID addConnection(Node* left, PortIndex leftIdx, Node* right, PortIndex rightIdx, TypeConverter converter);

  using SharedConnection = std::shared_ptr<Connection>;
  using UniqueNode       = std::unique_ptr<Node>;

  std::unordered_map<ConnectionID, SharedConnection> _connections;
  std::unordered_map<QUuid, UniqueNode>              _nodes;
  std::shared_ptr<DataModelRegistry>                 _registry;
};


DataFlowScene::DataFlowScene(std::shared_ptr<DataModelRegistry> registry, QObject* parent) 
  : FlowScene(new DataFlowModel(std::move(registry)), parent) {
  _dataFlowModel = static_cast<DataFlowModel*>(model());
}

std::shared_ptr<Connection>
DataFlowScene::
createConnection(Node& nodeIn,
                  PortIndex portIndexIn,
                  Node& nodeOut,
                  PortIndex portIndexOut,
                  TypeConverter const& converter) {
  
  auto connid = _dataFlowModel->addConnection(&nodeOut, portIndexOut, &nodeIn, portIndexIn, converter);

  return _dataFlowModel->_connections[connid];
}

std::shared_ptr<Connection>
DataFlowScene::
restoreConnection(QJsonObject const &connectionJson)
{
  QUuid nodeInId  = QUuid(connectionJson["in_id"].toString());
  QUuid nodeOutId = QUuid(connectionJson["out_id"].toString());

  PortIndex portIndexIn  = connectionJson["in_index"].toInt();
  PortIndex portIndexOut = connectionJson["out_index"].toInt();


  ConnectionID connId;
  connId.lNodeID = nodeOutId;
  connId.rNodeID = nodeInId;
  
  connId.lPortID = portIndexOut;
  connId.rPortID = portIndexIn;
  
  if (!_dataFlowModel->addConnection(_dataFlowModel->nodeIndex(nodeOutId), connId.lPortID, _dataFlowModel->nodeIndex(nodeInId), connId.rPortID)) 
    return nullptr;

  return _dataFlowModel->_connections[connId];
  
}

void
DataFlowScene::
deleteConnection(Connection& connection)
{
  auto deleted = _dataFlowModel->removeConnection(
    _dataFlowModel->nodeIndex(connection.getNode(PortType::Out)->id()),
    connection.getPortIndex(PortType::Out),
    _dataFlowModel->nodeIndex(connection.getNode(PortType::In)->id()),
    connection.getPortIndex(PortType::Out));
  Q_ASSERT(deleted);
}

Node&
DataFlowScene::
createNode(std::unique_ptr<NodeDataModel> && dataModel)
{
  auto uid = _dataFlowModel->addNode(std::move(dataModel), {0.0, 0.0});

  return *_dataFlowModel->_nodes[uid];
}

Node&
DataFlowScene::
restoreNode(QJsonObject const& nodeJson)
{
  QString modelName = nodeJson["model"].toObject()["name"].toString();

  auto uid = QUuid(nodeJson["id"].toString());
 
  // set initial point to 0, 0, will be updated in Node::restore
  _dataFlowModel->addNode(modelName, QPointF(0, 0), uid);

  auto& node = *_dataFlowModel->_nodes[uid];
  node.restore(nodeJson);
  
  return node;
}

void 
DataFlowScene::
removeNode(Node& node)
{
  model()->removeNodeWithConnections(model()->nodeIndex(node.id()));
}

DataModelRegistry&
DataFlowScene::
registry() const
{
  return *_dataFlowModel->_registry;
}

void
DataFlowScene::
setRegistry(std::shared_ptr<DataModelRegistry> registry)
{
  _dataFlowModel->_registry = registry;
}

void
DataFlowScene::
iterateOverNodes(std::function<void(Node*)> const& visitor) 
{
  for (auto const& node : _dataFlowModel->_nodes) {
    visitor(node.second.get());
  }
}

void
DataFlowScene::
iterateOverNodeData(std::function<void(NodeDataModel*)> const& visitor)
{
  for (auto const& node : _dataFlowModel->_nodes) {
    visitor(node.second->nodeDataModel());
  }
}

void
DataFlowScene::
iterateOverNodeDataDependentOrder(std::function<void(NodeDataModel*)> const& visitor)
{
  std::set<QUuid> visitedNodesSet;

  //A leaf node is a node with no input ports, or all possible input ports empty
  auto isNodeLeaf =
    [](Node const &node, NodeDataModel const &model)
    {
      for (unsigned int i = 0; i < model.nPorts(PortType::In); ++i)
      {
        auto connections = node.connections(PortType::In, i);
        if (!connections.empty())
        {
          return false;
        }
      }

      return true;
    };

  //Iterate over "leaf" nodes
  for (auto const &_node : _dataFlowModel->_nodes)
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
        auto connections = node.connections(PortType::In, i);

        for (auto& conn : connections)
        {
          if (visitedNodesSet.find(conn->getNode(PortType::Out)->id()) == visitedNodesSet.end())
          {
            return false;
          }
        }
      }

      return true;
    };

  //Iterate over dependent nodes
  while (_dataFlowModel->_nodes.size() != visitedNodesSet.size())
  {
    for (auto const &_node : _dataFlowModel->_nodes)
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
DataFlowScene::
getNodePosition(Node const& node) const 
{
  return _dataFlowModel->nodeLocation(_dataFlowModel->nodeIndex(node.id()));
}

void
DataFlowScene::
setNodePosition(Node& node, QPointF const& pos) const
{
  _dataFlowModel->moveNode(_dataFlowModel->nodeIndex(node.id()), pos);
}


std::unordered_map<QUuid, std::unique_ptr<Node> > const &
DataFlowScene::
nodes() const
{
  return _dataFlowModel->_nodes;
}

std::unordered_map<ConnectionID, std::shared_ptr<Connection>> const &
DataFlowScene::
connections() const 
{
  return _dataFlowModel->_connections;
}

std::vector<Node*>
DataFlowScene::
selectedNodes() const
{
  auto ids = FlowScene::selectedNodes();
  std::vector<Node*> ret;
  ret.reserve(ids.size());

  std::transform(ids.begin(), ids.end(), std::back_inserter(ret), [this](NodeIndex const& id) {
    return _dataFlowModel->_nodes.find(id.id())->second.get();
  });

  return ret;
}

void
DataFlowScene::
clearScene() {
  // delete all the nodes
  while(!_dataFlowModel->_nodes.empty()) {
    removeNode(*_dataFlowModel->_nodes.begin()->second);
  }
}

void
DataFlowScene::
save() const
{
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
      file.write(saveToMemory());
    }
  }
}


void
DataFlowScene::
load()
{
  clearScene();

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

  loadFromMemory(wholeFile);
}


QByteArray
DataFlowScene::
saveToMemory() const
{
  QJsonObject sceneJson;

  QJsonArray nodesJsonArray;

  for (auto const & pair : _dataFlowModel->_nodes)
  {
    auto const &node = pair.second;

    nodesJsonArray.append(node->save());
  }

  sceneJson["nodes"] = nodesJsonArray;

  QJsonArray connectionJsonArray;
  for (auto const & pair : _dataFlowModel->_connections)
  {
    auto const &connection = pair.second;

    QJsonObject connectionJson = connection->save();

    if (!connectionJson.isEmpty())
      connectionJsonArray.append(connectionJson);
  }

  sceneJson["connections"] = connectionJsonArray;

  QJsonDocument document(sceneJson);

  return document.toJson();
}


void
DataFlowScene::
loadFromMemory(const QByteArray& data)
{
  QJsonObject const jsonDocument = QJsonDocument::fromJson(data).object();

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
}

DataFlowScene::DataFlowModel::DataFlowModel(std::shared_ptr<DataModelRegistry> registry) 
  : _registry(std::move(registry)) {
}

// FlowSceneModel read interface
QStringList DataFlowScene::DataFlowModel::modelRegistry() const {
  QStringList list;
  for (const auto& item : _registry->registeredModelCreators()) {
    list << item.first;
  }
  return list;
}
QString DataFlowScene::DataFlowModel::nodeTypeCatergory(QString const& name) const {
  auto iter = _registry->registeredModelsCategoryAssociation().find(name);
  
  if (iter != _registry->registeredModelsCategoryAssociation().end()) {
    return iter->second;
  }
  return {};
}
bool DataFlowScene::DataFlowModel::getTypeConvertable(TypeConverterId const& id) const {
  return static_cast<bool>(_registry->getTypeConverter(id.first, id.second));
}
QList<QUuid> DataFlowScene::DataFlowModel::nodeUUids() const {
  QList<QUuid> ret;

  // extract the keys
  std::transform(_nodes.begin(), _nodes.end(), std::back_inserter(ret), [](const auto& pair) {
    return pair.first;
  });

  return ret;
}
NodeIndex DataFlowScene::DataFlowModel::nodeIndex(const QUuid& ID) const {
  auto iter = _nodes.find(ID);
  if (iter == _nodes.end()) return {};
  
  return createIndex(ID, iter->second.get());
}
QString DataFlowScene::DataFlowModel::nodeTypeIdentifier(NodeIndex const& index) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->nodeDataModel()->name();
}
QString DataFlowScene::DataFlowModel::nodeCaption(NodeIndex const& index) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  if (!node->nodeDataModel()->captionVisible()) return {};
  
  return node->nodeDataModel()->caption();
}
QPointF DataFlowScene::DataFlowModel::nodeLocation(NodeIndex const& index) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->position();
}
QWidget* DataFlowScene::DataFlowModel::nodeWidget(NodeIndex const& index) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->nodeDataModel()->embeddedWidget();
}
bool DataFlowScene::DataFlowModel::nodeResizable(NodeIndex const& index) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->nodeDataModel()->resizable();
}
NodeValidationState DataFlowScene::DataFlowModel::nodeValidationState(NodeIndex const& index) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->nodeDataModel()->validationState();
}
QString DataFlowScene::DataFlowModel::nodeValidationMessage(NodeIndex const& index) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->nodeDataModel()->validationMessage();
}
NodePainterDelegate* DataFlowScene::DataFlowModel::nodePainterDelegate(NodeIndex const& index) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->nodeDataModel()->painterDelegate();
}
unsigned int DataFlowScene::DataFlowModel::nodePortCount(NodeIndex const& index, PortType portType) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->nodeDataModel()->nPorts(portType);
}
QString DataFlowScene::DataFlowModel::nodePortCaption(NodeIndex const& index, PortIndex pIndex, PortType portType) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->nodeDataModel()->portCaption(portType, pIndex);
}
NodeDataType DataFlowScene::DataFlowModel::nodePortDataType(NodeIndex const& index, PortIndex pIndex, PortType portType) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  return node->nodeDataModel()->dataType(portType, pIndex);
}
ConnectionPolicy DataFlowScene::DataFlowModel::nodePortConnectionPolicy(NodeIndex const& index, PortIndex pIndex, PortType portType) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  if (portType == PortType::In) {
    return ConnectionPolicy::One;
  }
  return node->nodeDataModel()->portOutConnectionPolicy(pIndex);
}
std::vector<std::pair<NodeIndex, PortIndex>> DataFlowScene::DataFlowModel::nodePortConnections(NodeIndex const& index, PortIndex id, PortType portType) const {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  std::vector<std::pair<NodeIndex, PortIndex>> ret;
  // construct connections
  for (const auto& conn : node->connections(portType, id)) {
    ret.emplace_back(nodeIndex(conn->getNode(oppositePort(portType))->id()), conn->getPortIndex(oppositePort(portType)));
  }
  return ret;
}

// FlowSceneModel write interface
bool DataFlowScene::DataFlowModel::removeConnection(NodeIndex const& leftNodeIdx, PortIndex leftPortID, NodeIndex const& rightNodeIdx, PortIndex rightPortID) {
  Q_ASSERT(leftNodeIdx.isValid());
  Q_ASSERT(rightNodeIdx.isValid());
  
  auto* leftNode = static_cast<Node*>(leftNodeIdx.internalPointer());
  auto* rightNode = static_cast<Node*>(rightNodeIdx.internalPointer());
  
  ConnectionID connID;
  connID.lNodeID = leftNodeIdx.id();
  connID.rNodeID = rightNodeIdx.id();
  connID.lPortID = leftPortID;
  connID.rPortID = rightPortID;

  _connections[connID]->propagateEmptyData();
  
  // remove it from the nodes
  auto& leftConns = leftNode->connections(PortType::Out, leftPortID);
  auto iter = std::find_if(leftConns.begin(), leftConns.end(), [&](Connection* conn){ return conn->id() == connID; });
  Q_ASSERT(iter != leftConns.end());
  leftConns.erase(iter);
  
  auto& rightConns = rightNode->connections(PortType::In, rightPortID);
  iter = std::find_if(rightConns.begin(), rightConns.end(), [&](Connection* conn){ return conn->id() == connID; });
  Q_ASSERT(iter != rightConns.end());
  rightConns.erase(iter);
  
  // remove it from the map
  _connections.erase(connID);

  // tell the view
  emit connectionRemoved(leftNodeIdx, leftPortID, rightNodeIdx, rightPortID);
  
  return true;
}
bool DataFlowScene::DataFlowModel::addConnection(NodeIndex const& leftNodeIdx, PortIndex leftPortID, NodeIndex const& rightNodeIdx, PortIndex rightPortID) {
  Q_ASSERT(leftNodeIdx.isValid());
  Q_ASSERT(rightNodeIdx.isValid());
  
  auto* leftNode = static_cast<Node*>(leftNodeIdx.internalPointer());
  auto* rightNode = static_cast<Node*>(rightNodeIdx.internalPointer());

  // type conversions
  TypeConverter conv = {};
  auto ltype = nodePortDataType(leftNodeIdx, leftPortID, PortType::Out);
  auto rtype = nodePortDataType(rightNodeIdx, rightPortID, PortType::In);
  if (ltype.id != rtype.id) {
    conv = _registry->getTypeConverter(ltype, rtype);
  }

  addConnection(leftNode, leftPortID, rightNode, rightPortID, conv);

  return true;
}
bool DataFlowScene::DataFlowModel::removeNode(NodeIndex const& index) {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  
  // make sure there are no connections left
#ifndef QT_NO_DEBUG
  for (auto idx = 0u; idx < node->nodeDataModel()->nPorts(PortType::In); ++idx) {
    Q_ASSERT(node->connections(PortType::In, idx).empty());
  }
  for (auto idx = 0u; idx < node->nodeDataModel()->nPorts(PortType::Out); ++idx) {
    Q_ASSERT(node->connections(PortType::Out, idx).empty());
  }
#endif

  // remove it from the map
  _nodes.erase(index.id());
  
  // tell the view
  emit nodeRemoved(index.id());

  return true;
}
QUuid DataFlowScene::DataFlowModel::addNode(const QString& typeID, QPointF const& location) {
  auto nodeid = QUuid::createUuid();
  addNode(typeID, location, nodeid);
  return nodeid;
}
QUuid DataFlowScene::DataFlowModel::addNode(QString const& typeID, QPointF const& location, QUuid const& nodeid) {
  // create the NodeDataModel
  auto model = _registry->create(typeID);
  if (!model) {
    return {};
  }
  return addNode(std::move(model), location, nodeid);
}
QUuid DataFlowScene::DataFlowModel::addNode(std::unique_ptr<NodeDataModel>&& model, QPointF const& location, QUuid const& nodeid) {

  connect(model.get(), &NodeDataModel::dataUpdated, this, [this, nodeid](PortIndex){ emit nodeValidationUpdated(nodeIndex(nodeid)); });

  // create a node
  auto node = std::make_unique<Node>(std::move(model), nodeid);

  node->setPosition(location);
  
  // cache the pointer so the connection can be made
  auto nodePtr = node.get();
  
  // add it to the map
  _nodes[nodeid] = std::move(node);
  
  // connect to the geometry gets updated
  connect(nodePtr, &Node::positionChanged, this, [this, nodeid](QPointF const&){ emit nodeMoved(nodeIndex(nodeid)); });
  
  // tell the view
  emit nodeAdded(nodeid);

  return nodeid;
}
ConnectionID DataFlowScene::DataFlowModel::addConnection(Node* leftNode, PortIndex leftPortID, Node* rightNode, PortIndex rightPortID, TypeConverter conv) {

  ConnectionID connID;
  connID.lNodeID = leftNode->id();
  connID.rNodeID = rightNode->id();
  connID.lPortID = leftPortID;
  connID.rPortID = rightPortID;

  // create the connection
  auto conn = std::make_shared<Connection>(*rightNode, rightPortID, *leftNode, leftPortID, conv);
  _connections[connID] = conn;
  
  // add it to the nodes
  leftNode->connections(PortType::Out, leftPortID).push_back(conn.get());
  rightNode->connections(PortType::In, rightPortID).push_back(conn.get());

  // process data
  conn->getNode(PortType::Out)->onDataUpdated(conn->getPortIndex(PortType::Out));
  
  // tell the view the connection was added
  emit connectionAdded(nodeIndex(leftNode->id()), leftPortID, nodeIndex(rightNode->id()), rightPortID);

  return connID;
}

bool DataFlowScene::DataFlowModel::moveNode(NodeIndex const& index, QPointF newLocation) {
  Q_ASSERT(index.isValid());
  
  auto* node = static_cast<Node*>(index.internalPointer());
  node->setPosition(newLocation);
  
  // no need to emit, it's done by the function already
  return true;
}
