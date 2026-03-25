#include "BasicGraphicsScene.hpp"

#include "AbstractNodeGeometry.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "DataFlowGraphModel.hpp"
#include "DefaultConnectionPainter.hpp"
#include "DefaultHorizontalNodeGeometry.hpp"
#include "DefaultNodePainter.hpp"
#include "DefaultVerticalNodeGeometry.hpp"
#include "GraphicsView.hpp"
#include "NodeDelegateModel.hpp"
#include "NodeGraphicsObject.hpp"

#include <QUndoStack>

#include <QtWidgets/QGraphicsSceneMoveEvent>
#include <QtCore/QtGlobal>

#include <queue>

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

AbstractGraphModel const &BasicGraphicsScene::graphModel() const noexcept
{
    return _graphModel;
}

AbstractGraphModel &BasicGraphicsScene::graphModel() noexcept
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

void BasicGraphicsScene::setNodeGeometry(std::unique_ptr<AbstractNodeGeometry> newGeom)
{
    _nodeGeometry = std::move(newGeom);
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
    std::vector<NodeId> nodeIds;
    auto const &allNodeIds = graphModel().allNodeIds();
    nodeIds.reserve(allNodeIds.size());
    for (auto const nodeId : allNodeIds) {
        nodeIds.push_back(nodeId);
    }

    for (auto const nodeId : nodeIds) {
        graphModel().deleteNode(nodeId);
    }
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
    auto const &allNodeIds = _graphModel.allNodeIds();

    // First create all the nodes.
    for (NodeId const nodeId : allNodeIds) {
        _nodeGraphicsObjects[nodeId] = std::make_unique<NodeGraphicsObject>(*this, nodeId);
        _nodeGraphicsObjects[nodeId]->updateValidationTooltip();
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
        node->updateValidationTooltip();
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
    _nodeGraphicsObjects[nodeId]->updateValidationTooltip();

    Q_EMIT modified(this);
}

void BasicGraphicsScene::onNodePositionUpdated(NodeId const nodeId)
{
    auto node = nodeGraphicsObject(nodeId);
    if (node) {
        node->setPos(_graphModel.nodeData(nodeId, NodeRole::Position).value<QPointF>());
        if (auto group = node->nodeGroup().lock()) {
            group->groupGraphicsObject().updateGroupGeometry();
        }
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

        node->updateValidationTooltip();
        if (auto group = node->nodeGroup().lock()) {
            group->groupGraphicsObject().updateGroupGeometry();
        }
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

void BasicGraphicsScene::freezeModelAndConnections(bool isFreeze)
{
    for (QGraphicsItem *item : selectedItems()) {
        if (auto n = qgraphicsitem_cast<NodeGraphicsObject *>(item)) {
            int portCount = graphModel().nodeData(n->nodeId(), NodeRole::OutPortCount).toInt();
            for (int i = 0; i < portCount; i++) {
                auto const &graphConnections = graphModel().connections(n->nodeId(),
                                                                        QtNodes::PortType::Out,
                                                                        QtNodes::PortIndex(i));

                for (auto const &c : graphConnections) {
                    if (auto *cgo = connectionGraphicsObject(c)) {
                        cgo->connectionState().setFrozen(isFreeze);
                        cgo->update();
                    }
                }
            }

            if (auto *dfModel = dynamic_cast<DataFlowGraphModel *>(&graphModel())) {
                if (auto *delegate = dfModel->delegateModel<NodeDelegateModel>(n->nodeId())) {
                    delegate->setFrozenState(isFreeze);
                }
            }
        }
    }
}

namespace {
template<typename T>
std::vector<T *> selectedItemsOfType(QGraphicsScene const *scene)
{
    QList<QGraphicsItem *> graphicsItems = scene->selectedItems();

    std::vector<T *> result;
    result.reserve(graphicsItems.size());

    for (QGraphicsItem *item : graphicsItems) {
        if (auto typed = qgraphicsitem_cast<T *>(item)) {
            result.push_back(typed);
        }
    }

    return result;
}
} // namespace

std::vector<NodeGraphicsObject *> BasicGraphicsScene::selectedNodes() const
{
    return selectedItemsOfType<NodeGraphicsObject>(this);
}

std::vector<GroupGraphicsObject *> BasicGraphicsScene::selectedGroups() const
{
    if (!_groupingEnabled)
        return {};

    return selectedItemsOfType<GroupGraphicsObject>(this);
}

std::weak_ptr<QtNodes::NodeGroup> BasicGraphicsScene::createGroupFromSelection(QString groupName)
{
    if (!_groupingEnabled)
        return std::weak_ptr<NodeGroup>();

    auto nodes = selectedNodes();
    return createGroup(nodes, groupName);
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

} // namespace QtNodes
