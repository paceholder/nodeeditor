#include "BasicGraphicsScene.hpp"

#include "AbstractNodeGeometry.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "DefaultConnectionPainter.hpp"
#include "DefaultHorizontalNodeGeometry.hpp"
#include "DefaultNodePainter.hpp"
#include "DefaultVerticalNodeGeometry.hpp"
#include "GraphicsView.hpp"
#include "NodeGraphicsObject.hpp"

#include <QUndoStack>

#include <QHeaderView>
#include <QLineEdit>
#include <QTreeWidget>
#include <QWidgetAction>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGraphicsSceneMoveEvent>

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QIODevice>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QJsonValue>
#include <QtCore/QString>
#include <QtCore/QtGlobal>

#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <utility>
#include <queue>

namespace {

using QtNodes::GroupId;
using QtNodes::InvalidGroupId;
using QtNodes::InvalidNodeId;
using QtNodes::NodeId;

NodeId jsonValueToNodeId(QJsonValue const &value)
{
    if (value.isDouble()) {
        return static_cast<NodeId>(value.toInt());
    }

    if (value.isString()) {
        auto const textValue = value.toString();

        bool ok = false;
        auto const numericValue = textValue.toULongLong(&ok, 10);
        if (ok) {
            return static_cast<NodeId>(numericValue);
        }

        QUuid uuidValue(textValue);
        if (!uuidValue.isNull()) {
            auto const bytes = uuidValue.toRfc4122();
            if (bytes.size() >= static_cast<int>(sizeof(quint32))) {
                QDataStream stream(bytes);
                quint32 value32 = 0U;
                stream >> value32;
                return static_cast<NodeId>(value32);
            }
        }
    }

    return InvalidNodeId;
}

} // namespace

namespace QtNodes {

BasicGraphicsScene::BasicGraphicsScene(AbstractGraphModel &graphModel, QObject *parent)
    : QGraphicsScene(parent)
    , _graphModel(graphModel)
    , _nodeGeometry(std::make_unique<DefaultHorizontalNodeGeometry>(_graphModel))
    , _nodePainter(std::make_unique<DefaultNodePainter>())
    , _connectionPainter(std::make_unique<DefaultConnectionPainter>())
    , _nodeDrag(false)
    , _undoStack(new QUndoStack(this))
    , _orientation(Qt::Horizontal)
    , _groupingEnabled(true)
{
    setItemIndexMethod(QGraphicsScene::NoIndex);

    connect(&_graphModel,
            &AbstractGraphModel::connectionCreated,
            this,
            &BasicGraphicsScene::onConnectionCreated);

    connect(&_graphModel,
            &AbstractGraphModel::connectionDeleted,
            this,
            &BasicGraphicsScene::onConnectionDeleted);

    connect(&_graphModel,
            &AbstractGraphModel::nodeCreated,
            this,
            &BasicGraphicsScene::onNodeCreated);

    connect(&_graphModel,
            &AbstractGraphModel::nodeDeleted,
            this,
            &BasicGraphicsScene::onNodeDeleted);

    connect(&_graphModel,
            &AbstractGraphModel::nodePositionUpdated,
            this,
            &BasicGraphicsScene::onNodePositionUpdated);

    connect(&_graphModel,
            &AbstractGraphModel::nodeUpdated,
            this,
            &BasicGraphicsScene::onNodeUpdated);

    connect(this, &BasicGraphicsScene::nodeClicked, this, &BasicGraphicsScene::onNodeClicked);

    connect(&_graphModel, &AbstractGraphModel::modelReset, this, &BasicGraphicsScene::onModelReset);

    traverseGraphAndPopulateGraphicsObjects();
}

BasicGraphicsScene::~BasicGraphicsScene() = default;

AbstractGraphModel const &BasicGraphicsScene::graphModel() const
{
    return _graphModel;
}

AbstractGraphModel &BasicGraphicsScene::graphModel()
{
    return _graphModel;
}

AbstractNodeGeometry const &BasicGraphicsScene::nodeGeometry() const
{
    return *_nodeGeometry;
}

AbstractNodeGeometry &BasicGraphicsScene::nodeGeometry()
{
    return *_nodeGeometry;
}

AbstractNodePainter &BasicGraphicsScene::nodePainter()
{
    return *_nodePainter;
}

AbstractConnectionPainter &BasicGraphicsScene::connectionPainter()
{
    return *_connectionPainter;
}

void BasicGraphicsScene::setNodePainter(std::unique_ptr<AbstractNodePainter> newPainter)
{
    _nodePainter = std::move(newPainter);
}

void BasicGraphicsScene::setConnectionPainter(std::unique_ptr<AbstractConnectionPainter> newPainter)
{
    _connectionPainter = std::move(newPainter);
}

QUndoStack &BasicGraphicsScene::undoStack()
{
    return *_undoStack;
}

void BasicGraphicsScene::setGroupingEnabled(bool enabled)
{
    if (_groupingEnabled == enabled)
        return;

    if (!enabled) {
        for (auto &groupEntry : _groups) {
            auto &group = groupEntry.second;
            if (!group)
                continue;

            for (auto *node : group->childNodes()) {
                if (!node)
                    continue;

                node->unsetNodeGroup();
                node->lock(false);
            }
        }

        _groups.clear();
        _nextGroupId = 0;
    }

    _groupingEnabled = enabled;
}

std::unique_ptr<ConnectionGraphicsObject> const &BasicGraphicsScene::makeDraftConnection(
    ConnectionId const incompleteConnectionId)
{
    _draftConnection = std::make_unique<ConnectionGraphicsObject>(*this, incompleteConnectionId);

    _draftConnection->grabMouse();

    return _draftConnection;
}

void BasicGraphicsScene::resetDraftConnection()
{
    _draftConnection.reset();
}

void BasicGraphicsScene::clearScene()
{
    auto const &allNodeIds = graphModel().allNodeIds();

    for (auto nodeId : allNodeIds) {
        graphModel().deleteNode(nodeId);
    }
}

std::vector<std::shared_ptr<ConnectionId>> BasicGraphicsScene::connectionsWithinGroup(GroupId groupID)
{
    if (!_groupingEnabled)
        return {};

    std::vector<std::shared_ptr<ConnectionId>> ret{};

    for (auto const &connection : _connectionGraphicsObjects) {
        auto outNode = nodeGraphicsObject(connection.first.outNodeId);
        auto inNode = nodeGraphicsObject(connection.first.inNodeId);
        if (outNode && inNode) {
            auto group1 = outNode->nodeGroup().lock();
            auto group2 = inNode->nodeGroup().lock();
            if (group1 && group2 && group1->id() == group2->id() && group1->id() == groupID) {
                ret.push_back(std::make_shared<ConnectionId>(connection.first));
            }
        }
    }

    return ret;
}

NodeGraphicsObject *BasicGraphicsScene::nodeGraphicsObject(NodeId nodeId)
{
    NodeGraphicsObject *ngo = nullptr;
    auto it = _nodeGraphicsObjects.find(nodeId);
    if (it != _nodeGraphicsObjects.end()) {
        ngo = it->second.get();
    }

    return ngo;
}

ConnectionGraphicsObject *BasicGraphicsScene::connectionGraphicsObject(ConnectionId connectionId)
{
    ConnectionGraphicsObject *cgo = nullptr;
    auto it = _connectionGraphicsObjects.find(connectionId);
    if (it != _connectionGraphicsObjects.end()) {
        cgo = it->second.get();
    }

    return cgo;
}

void BasicGraphicsScene::setOrientation(Qt::Orientation const orientation)
{
    if (_orientation != orientation) {
        _orientation = orientation;

        switch (_orientation) {
        case Qt::Horizontal:
            _nodeGeometry = std::make_unique<DefaultHorizontalNodeGeometry>(_graphModel);
            break;

        case Qt::Vertical:
            _nodeGeometry = std::make_unique<DefaultVerticalNodeGeometry>(_graphModel);
            break;
        }

        onModelReset();
    }
}

QMenu *BasicGraphicsScene::createSceneMenu(QPointF const scenePos)
{
    Q_UNUSED(scenePos);
    return nullptr;
}

void BasicGraphicsScene::traverseGraphAndPopulateGraphicsObjects()
{
    auto allNodeIds = _graphModel.allNodeIds();

    // First create all the nodes.
    for (NodeId const nodeId : allNodeIds) {
        _nodeGraphicsObjects[nodeId] = std::make_unique<NodeGraphicsObject>(*this, nodeId);
    }

    // Then for each node check output connections and insert them.
    for (NodeId const nodeId : allNodeIds) {
        auto nOutPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::OutPortCount);

        for (PortIndex index = 0; index < nOutPorts; ++index) {
            auto const &outConnectionIds = _graphModel.connections(nodeId, PortType::Out, index);

            for (auto cid : outConnectionIds) {
                _connectionGraphicsObjects[cid] = std::make_unique<ConnectionGraphicsObject>(*this,
                                                                                             cid);
            }
        }
    }
}

void BasicGraphicsScene::updateAttachedNodes(ConnectionId const connectionId,
                                             PortType const portType)
{
    auto node = nodeGraphicsObject(getNodeId(portType, connectionId));

    if (node) {
        node->update();
    }
}

void BasicGraphicsScene::onConnectionDeleted(ConnectionId const connectionId)
{
    auto it = _connectionGraphicsObjects.find(connectionId);
    if (it != _connectionGraphicsObjects.end()) {
        _connectionGraphicsObjects.erase(it);
    }

    // TODO: do we need it?
    if (_draftConnection && _draftConnection->connectionId() == connectionId) {
        _draftConnection.reset();
    }

    updateAttachedNodes(connectionId, PortType::Out);
    updateAttachedNodes(connectionId, PortType::In);

    Q_EMIT modified(this);
}

void BasicGraphicsScene::onConnectionCreated(ConnectionId const connectionId)
{
    _connectionGraphicsObjects[connectionId]
        = std::make_unique<ConnectionGraphicsObject>(*this, connectionId);

    updateAttachedNodes(connectionId, PortType::Out);
    updateAttachedNodes(connectionId, PortType::In);

    Q_EMIT modified(this);
}

void BasicGraphicsScene::onNodeDeleted(NodeId const nodeId)
{
    auto it = _nodeGraphicsObjects.find(nodeId);
    if (it != _nodeGraphicsObjects.end()) {
        removeNodeFromGroup(nodeId);
        _nodeGraphicsObjects.erase(it);

        Q_EMIT modified(this);
    }
}

void BasicGraphicsScene::onNodeCreated(NodeId const nodeId)
{
    _nodeGraphicsObjects[nodeId] = std::make_unique<NodeGraphicsObject>(*this, nodeId);

    Q_EMIT modified(this);
}

void BasicGraphicsScene::onNodePositionUpdated(NodeId const nodeId)
{
    auto node = nodeGraphicsObject(nodeId);
    if (node) {
        node->setPos(_graphModel.nodeData(nodeId, NodeRole::Position).value<QPointF>());
        node->update();
        _nodeDrag = true;
    }
}

void BasicGraphicsScene::onNodeUpdated(NodeId const nodeId)
{
    auto node = nodeGraphicsObject(nodeId);

    if (node) {
        node->setGeometryChanged();

        _nodeGeometry->recomputeSize(nodeId);

        node->updateQWidgetEmbedPos();
        node->update();
        node->moveConnections();
    }
}

void BasicGraphicsScene::onNodeClicked(NodeId const nodeId)
{
    if (_nodeDrag) {
        Q_EMIT nodeMoved(nodeId, _graphModel.nodeData(nodeId, NodeRole::Position).value<QPointF>());
        Q_EMIT modified(this);
    }
    _nodeDrag = false;
}

void BasicGraphicsScene::onModelReset()
{
    _connectionGraphicsObjects.clear();
    _nodeGraphicsObjects.clear();

    clear();

    traverseGraphAndPopulateGraphicsObjects();
}

std::weak_ptr<NodeGroup> BasicGraphicsScene::createGroup(std::vector<NodeGraphicsObject *> &nodes,
                                                         QString groupName,
                                                         GroupId groupId)
{
    if (!_groupingEnabled)
        return std::weak_ptr<NodeGroup>();

    if (nodes.empty())
        return std::weak_ptr<NodeGroup>();

    for (auto *node : nodes) {
        if (!node->nodeGroup().expired())
            removeNodeFromGroup(node->nodeId());
    }

    if (groupName.isEmpty()) {
        groupName = "Group " + QString::number(NodeGroup::groupCount());
    }

    if (groupId == InvalidGroupId) {
        groupId = nextGroupId();
    } else {
        if (_groups.count(groupId) != 0) {
            throw std::runtime_error("Group identifier collision");
        }

        if (groupId >= _nextGroupId && _nextGroupId != InvalidGroupId) {
            _nextGroupId = groupId + 1;
        }
    }

    auto group = std::make_shared<NodeGroup>(nodes, groupId, groupName, this);
    auto ggo = std::make_unique<GroupGraphicsObject>(*this, *group);

    group->setGraphicsObject(std::move(ggo));

    for (auto &nodePtr : nodes) {
        auto node = _nodeGraphicsObjects[nodePtr->nodeId()].get();

        node->setNodeGroup(group);
    }

    std::weak_ptr<NodeGroup> groupWeakPtr = group;

    _groups[group->id()] = std::move(group);

    return groupWeakPtr;
}

std::vector<NodeGraphicsObject *> BasicGraphicsScene::selectedNodes() const
{
    QList<QGraphicsItem *> graphicsItems = selectedItems();

    std::vector<NodeGraphicsObject *> result;
    result.reserve(graphicsItems.size());

    for (QGraphicsItem *item : graphicsItems) {
        auto ngo = qgraphicsitem_cast<NodeGraphicsObject *>(item);

        if (ngo) {
            result.push_back(ngo);
        }
    }

    return result;
}

std::vector<GroupGraphicsObject *> BasicGraphicsScene::selectedGroups() const
{
    if (!_groupingEnabled)
        return {};

    QList<QGraphicsItem *> graphicsItems = selectedItems();

    std::vector<GroupGraphicsObject *> result;
    result.reserve(graphicsItems.size());

    for (QGraphicsItem *item : graphicsItems) {
        auto ngo = qgraphicsitem_cast<GroupGraphicsObject *>(item);

        if (ngo) {
            result.push_back(ngo);
        }
    }

    return result;
}

void BasicGraphicsScene::addNodeToGroup(NodeId nodeId, GroupId groupId)
{
    if (!_groupingEnabled)
        return;

    auto groupIt = _groups.find(groupId);
    auto nodeIt = _nodeGraphicsObjects.find(nodeId);
    if (groupIt == _groups.end() || nodeIt == _nodeGraphicsObjects.end())
        return;

    auto group = groupIt->second;
    auto node = nodeIt->second.get();
    group->addNode(node);
    node->setNodeGroup(group);
}

void BasicGraphicsScene::removeNodeFromGroup(NodeId nodeId)
{
    if (!_groupingEnabled)
        return;

    auto nodeIt = _nodeGraphicsObjects.find(nodeId);
    if (nodeIt == _nodeGraphicsObjects.end())
        return;

    auto group = nodeIt->second->nodeGroup().lock();
    if (group) {
        group->removeNode(nodeIt->second.get());
        if (group->empty()) {
            _groups.erase(group->id());
        }
    }
    nodeIt->second->unsetNodeGroup();
    nodeIt->second->lock(false);
}

std::weak_ptr<QtNodes::NodeGroup> BasicGraphicsScene::createGroupFromSelection(QString groupName)
{
    if (!_groupingEnabled)
        return std::weak_ptr<NodeGroup>();

    auto nodes = selectedNodes();
    return createGroup(nodes, groupName);
}

NodeGraphicsObject &BasicGraphicsScene::loadNodeToMap(QJsonObject nodeJson, bool keepOriginalId)
{
    NodeId newNodeId = InvalidNodeId;

    if (keepOriginalId) {
        newNodeId = jsonValueToNodeId(nodeJson["id"]);
    }

    if (newNodeId == InvalidNodeId) {
        newNodeId = _graphModel.newNodeId();
        nodeJson["id"] = static_cast<qint64>(newNodeId);
    }

    _graphModel.loadNode(nodeJson);

    auto *nodeObject = nodeGraphicsObject(newNodeId);
    if (!nodeObject) {
        auto graphicsObject = std::make_unique<NodeGraphicsObject>(*this, newNodeId);
        nodeObject = graphicsObject.get();
        _nodeGraphicsObjects[newNodeId] = std::move(graphicsObject);
    }

    return *nodeObject;
}

void BasicGraphicsScene::loadConnectionToMap(QJsonObject const &connectionJson,
                                             std::unordered_map<NodeId, NodeId> const &nodeIdMap)
{
    ConnectionId connId = fromJson(connectionJson);

    auto const outIt = nodeIdMap.find(connId.outNodeId);
    auto const inIt = nodeIdMap.find(connId.inNodeId);

    if (outIt == nodeIdMap.end() || inIt == nodeIdMap.end()) {
        return;
    }

    ConnectionId remapped{outIt->second, connId.outPortIndex, inIt->second, connId.inPortIndex};

    if (_graphModel.connectionExists(remapped)) {
        return;
    }

    if (_graphModel.connectionPossible(remapped)) {
        _graphModel.addConnection(remapped);
    }
}

std::pair<std::weak_ptr<NodeGroup>, std::unordered_map<GroupId, GroupId>>
BasicGraphicsScene::restoreGroup(QJsonObject const &groupJson)
{
    if (!_groupingEnabled)
        return {std::weak_ptr<NodeGroup>(), {}};

    // since the new nodes will have the same IDs as in the file and the connections
    // need these old IDs to be restored, we must create new IDs and map them to the
    // old ones so the connections are properly restored
    std::unordered_map<GroupId, GroupId> IDsMap{};
    std::unordered_map<NodeId, NodeId> nodeIdMap{};

    std::vector<NodeGraphicsObject *> group_children{};

    QJsonArray nodesJson = groupJson["nodes"].toArray();
    for (const QJsonValueRef nodeJson : nodesJson) {
        QJsonObject nodeObject = nodeJson.toObject();
        NodeId const oldNodeId = jsonValueToNodeId(nodeObject["id"]);

        NodeGraphicsObject &nodeRef = loadNodeToMap(nodeObject, false);
        NodeId const newNodeId = nodeRef.nodeId();

        if (oldNodeId != InvalidNodeId) {
            nodeIdMap.emplace(oldNodeId, newNodeId);
            IDsMap.emplace(static_cast<GroupId>(oldNodeId), static_cast<GroupId>(newNodeId));
        }

        group_children.push_back(&nodeRef);
    }

    QJsonArray connectionJsonArray = groupJson["connections"].toArray();
    for (auto connection : connectionJsonArray) {
        loadConnectionToMap(connection.toObject(), nodeIdMap);
    }

    return std::make_pair(createGroup(group_children, groupJson["name"].toString()), IDsMap);
}

std::unordered_map<GroupId, std::shared_ptr<NodeGroup>> const &BasicGraphicsScene::groups() const
{
    return _groups;
}

QMenu *BasicGraphicsScene::createStdMenu(QPointF const scenePos)
{
    Q_UNUSED(scenePos);
    QMenu *menu = new QMenu();

    if (_groupingEnabled) {
        QMenu *addToGroupMenu = menu->addMenu("Add to group...");

        for (const auto &groupMap : _groups) {
            auto groupPtr = groupMap.second;
            auto id = groupMap.first;

            if (!groupPtr)
                continue;

            auto groupName = groupPtr->name();

            QAction *groupAction = addToGroupMenu->addAction(groupName);

            for (const auto &node : selectedNodes()) {
                connect(groupAction, &QAction::triggered, [this, id, node]() {
                    this->addNodeToGroup(node->nodeId(), id);
                });
            }
        }

        QAction *createGroupAction = menu->addAction("Create group from selection");
        connect(createGroupAction, &QAction::triggered, [this]() { createGroupFromSelection(); });
    }

    QAction *copyAction = menu->addAction("Copy");
    copyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));

    QAction *cutAction = menu->addAction("Cut");
    cutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_X));

    connect(copyAction, &QAction::triggered, this, &BasicGraphicsScene::onCopySelectedObjects);

    connect(cutAction, &QAction::triggered, [this] {
        onCopySelectedObjects();
        onDeleteSelectedObjects();
    });

    menu->setAttribute(Qt::WA_DeleteOnClose);
    return menu;
}

QMenu *BasicGraphicsScene::createGroupMenu(QPointF const scenePos, GroupGraphicsObject *groupGo)
{
    Q_UNUSED(scenePos);
    QMenu *menu = new QMenu();

    QAction *saveGroup = nullptr;
    if (_groupingEnabled) {
        saveGroup = menu->addAction("Save group...");
    }

    QAction *copyAction = menu->addAction("Copy");
    copyAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_C));

    QAction *cutAction = menu->addAction("Cut");
    cutAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_X));

    if (saveGroup) {
        connect(saveGroup, &QAction::triggered, [this, groupGo] {
            saveGroupFile(groupGo->group().id());
        });
    }

    connect(copyAction, &QAction::triggered, this, &BasicGraphicsScene::onCopySelectedObjects);

    connect(cutAction, &QAction::triggered, [this] {
        onCopySelectedObjects();
        onDeleteSelectedObjects();
    });

    menu->setAttribute(Qt::WA_DeleteOnClose);
    return menu;
}

void BasicGraphicsScene::saveGroupFile(GroupId groupID)
{
    if (!_groupingEnabled)
        return;

    QString fileName = QFileDialog::getSaveFileName(nullptr,
                                                    tr("Save Node Group"),
                                                    QDir::homePath(),
                                                    tr("Node Group files (*.group)"));

    if (!fileName.isEmpty()) {
        if (!fileName.endsWith("group", Qt::CaseInsensitive))
            fileName += ".group";

        if (auto groupIt = _groups.find(groupID); groupIt != _groups.end()) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly)) {
                file.write(groupIt->second->saveToFile());
            } else {
                qDebug() << "Error saving group file!";
            }
        } else {
            qDebug() << "Error! Couldn't find group while saving.";
        }
    }
}

std::weak_ptr<NodeGroup> BasicGraphicsScene::loadGroupFile()
{
    if (!_groupingEnabled)
        return std::weak_ptr<NodeGroup>();

    QString fileName = QFileDialog::getOpenFileName(nullptr,
                                                    tr("Open Node Group"),
                                                    QDir::currentPath(),
                                                    tr("Node Group files (*.group)"));

    if (!QFileInfo::exists(fileName))
        return std::weak_ptr<NodeGroup>();

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Error loading group file!";
    }

    QDir d = QFileInfo(fileName).absoluteDir();
    QString absolute = d.absolutePath();
    QDir::setCurrent(absolute);

    QByteArray wholeFile = file.readAll();

    const QJsonObject fileJson = QJsonDocument::fromJson(wholeFile).object();

    return restoreGroup(fileJson).first;
}

GroupId BasicGraphicsScene::nextGroupId()
{
    if (_nextGroupId == InvalidGroupId) {
        throw std::runtime_error("No available group identifiers");
    }

    while (_groups.count(_nextGroupId) != 0) {
        ++_nextGroupId;
        if (_nextGroupId == InvalidGroupId) {
            throw std::runtime_error("No available group identifiers");
        }
    }

    GroupId const newId = _nextGroupId;
    ++_nextGroupId;
    return newId;
}

} // namespace QtNodes
