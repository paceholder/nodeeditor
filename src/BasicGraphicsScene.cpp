#include "BasicGraphicsScene.hpp"

#include "AbstractNodeGeometry.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "CommentGraphicsObject.hpp"
#include "DefaultConnectionPainter.hpp"
#include "DefaultHorizontalNodeGeometry.hpp"
#include "DefaultNodePainter.hpp"
#include "DefaultVerticalNodeGeometry.hpp"
#include "GraphicsView.hpp"
#include "NodeGraphicsObject.hpp"

#include <QUndoStack>

#include <QtWidgets/QFileDialog>
#include <QtWidgets/QGraphicsSceneMoveEvent>

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>
#include <QtCore/QtGlobal>

#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <utility>
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

QUuid BasicGraphicsScene::createCommentFromSelection()
{
    auto selectedItems = this->selectedItems();
    std::unordered_set<NodeId> selectedNodeIds;
    std::unordered_set<QUuid> selectedCommentIds;
    
    // First, collect all selected comments
    std::unordered_set<CommentGraphicsObject*> selectedComments;
    for (auto item : selectedItems) {
        if (auto commentGO = dynamic_cast<CommentGraphicsObject*>(item)) {
            selectedComments.insert(commentGO);
            selectedCommentIds.insert(commentGO->id());
        }
    }
    
    // Then collect nodes that should be directly added to the new parent comment
    // These are nodes that are selected but NOT in any non-selected comment
    for (auto item : selectedItems) {
        if (auto nodeGO = qgraphicsitem_cast<NodeGraphicsObject*>(item)) {
            NodeId nodeId = nodeGO->nodeId();
            bool nodeInNonSelectedComment = false;
            
            // Check if this node is in any comment that is NOT selected
            // (i.e., comments that won't become children of the new parent)
            for (auto const& [commentId, commentGO] : _commentGraphicsObjects) {
                if (commentGO && commentGO->containsNode(nodeId) && 
                    selectedCommentIds.find(commentId) == selectedCommentIds.end()) {
                    nodeInNonSelectedComment = true;
                    break;
                }
            }
            
            // Only add nodes that are not in non-selected comments
            // This means loose nodes OR nodes in comments that will become children
            if (!nodeInNonSelectedComment) {
                // But we still need to check if it's in a selected comment
                bool inSelectedComment = false;
                for (auto const& childId : selectedCommentIds) {
                    if (auto childComment = commentGraphicsObject(childId)) {
                        if (childComment->containsNode(nodeId)) {
                            inSelectedComment = true;
                            break;
                        }
                    }
                }
                
                // Only add if it's truly loose (not in any selected comment)
                if (!inSelectedComment) {
                    selectedNodeIds.insert(nodeId);
                }
            }
        }
    }
    
    // We need either comments or loose nodes to create a parent comment
    // If we only have nodes that are all inside selected comments, 
    // we still create a parent (the selected comments become children)
    if (selectedCommentIds.empty() && selectedNodeIds.empty()) {
        // Check if we at least have some selected items
        if (selectedItems.isEmpty()) {
            return QUuid();
        }
    }
    
    // Create new comment
    QUuid commentId = QUuid::createUuid();
    auto comment = std::make_unique<CommentGraphicsObject>(*this, commentId);
    comment->setCommentText("New Comment");
    
    // Set child comments first
    for (QUuid const &childId : selectedCommentIds) {
        comment->addChildComment(childId);
        if (auto childComment = commentGraphicsObject(childId)) {
            childComment->setParentComment(commentId);
        }
    }
    
    // Set grouped nodes (only loose nodes, not those in child comments)
    comment->setGroupedNodes(selectedNodeIds);
    
    // Find the lowest z-value among selected comments (children should be on top)
    qreal minZ = -1.0;
    for (QUuid const &childId : selectedCommentIds) {
        if (auto childComment = commentGraphicsObject(childId)) {
            minZ = qMin(minZ, childComment->zValue());
        }
    }
    
    // Set new parent comment below its children
    comment->setZValue(minZ - 0.1);
    
    // Calculate bounds before adding to scene
    comment->adjustBounds();
    
    // Add to scene
    addItem(comment.get());
    
    // Store in map
    _commentGraphicsObjects[commentId] = std::move(comment);
    
    // Clear selection and select the new comment
    clearSelection();
    _commentGraphicsObjects[commentId]->setSelected(true);
    
    Q_EMIT modified(this);
    
    return commentId;
}

void BasicGraphicsScene::deleteComment(QUuid const &commentId)
{
    auto it = _commentGraphicsObjects.find(commentId);
    if (it != _commentGraphicsObjects.end()) {
        removeItem(it->second.get());
        _commentGraphicsObjects.erase(it);
        Q_EMIT modified(this);
    }
}

void BasicGraphicsScene::addComment(QUuid const &commentId, std::unique_ptr<CommentGraphicsObject> comment)
{
    addItem(comment.get());
    _commentGraphicsObjects[commentId] = std::move(comment);
    Q_EMIT modified(this);
}

CommentGraphicsObject *BasicGraphicsScene::commentGraphicsObject(QUuid const &commentId)
{
    auto it = _commentGraphicsObjects.find(commentId);
    if (it != _commentGraphicsObjects.end()) {
        return it->second.get();
    }
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

} // namespace QtNodes
