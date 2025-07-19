#include "CommentGraphicsObject.hpp"

#include "BasicGraphicsScene.hpp"
#include "NodeGraphicsObject.hpp"
#include "StyleCollection.hpp"

#include <QtCore/QDebug>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QTextDocument>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QStyleOptionGraphicsItem>

#include <algorithm>

namespace QtNodes {

CommentGraphicsObject::CommentGraphicsObject(BasicGraphicsScene &scene,
                                             QUuid const &uid,
                                             QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , _scene(scene)
    , _uid(uid)
    , _color(QColor(180, 180, 180, 80)) // Light transparent grey
    , _parentCommentId()
    , _isResizing(false)
    , _isDragging(false)
{
    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);
    setAcceptHoverEvents(true);

    // Set z-value to be behind nodes but above the grid
    setZValue(-1.0);

    // Create text item for the comment
    _textItem = new QGraphicsTextItem(this);
    _textItem->setDefaultTextColor(Qt::black);
    _textItem->setPlainText("Comment");
    _textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
    _textItem->setPos(5, 5);
    
    // Ensure text is always on top within the comment
    _textItem->setZValue(1.0);

    // Connect text changes
    connect(_textItem->document(), &QTextDocument::contentsChanged, [this]() {
        Q_EMIT commentTextChanged(_textItem->toPlainText());
    });

    _rect = QRectF(0, 0, 200, 150);
}

CommentGraphicsObject::~CommentGraphicsObject() = default;

QRectF CommentGraphicsObject::boundingRect() const
{
    return _rect.adjusted(-2, -2, 2, 2);
}

void CommentGraphicsObject::paint(QPainter *painter,
                                  QStyleOptionGraphicsItem const *option,
                                  QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);

    // Draw shadow
    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0, 0, 0, 30));
    painter->drawRoundedRect(_rect.translated(2, 2), 5, 5);

    // Draw main rectangle
    painter->setPen(QPen(_color.darker(150), 2));
    painter->setBrush(_color);
    painter->drawRoundedRect(_rect, 5, 5);

    // Draw header background with full opacity
    QPainterPath headerPath;
    headerPath.addRoundedRect(QRectF(0, 0, _rect.width(), HEADER_HEIGHT), 5, 5);
    headerPath.addRect(QRectF(0, HEADER_HEIGHT/2, _rect.width(), HEADER_HEIGHT/2));

    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(_color.darker(110).red(), 
                            _color.darker(110).green(), 
                            _color.darker(110).blue(), 
                            200)); // More opaque header
    painter->drawPath(headerPath);

    // Draw separator line
    painter->setPen(QPen(_color.darker(150), 1));
    painter->drawLine(0, HEADER_HEIGHT, _rect.width(), HEADER_HEIGHT);
    
    // Draw a border around the header for better visibility
    painter->setPen(QPen(_color.darker(180), 1));
    painter->drawRoundedRect(QRectF(0, 0, _rect.width(), HEADER_HEIGHT), 5, 5);

    // Draw resize handle if hovered or selected
    if (isSelected() || acceptHoverEvents()) {
        painter->setPen(QPen(_color.darker(150), 1));
        painter->setBrush(Qt::NoBrush);
        
        // Bottom-right resize handle
        QPointF br = _rect.bottomRight();
        painter->drawLine(br - QPointF(RESIZE_HANDLE_SIZE, 0), br - QPointF(0, RESIZE_HANDLE_SIZE));
        painter->drawLine(br - QPointF(RESIZE_HANDLE_SIZE * 0.7, 0), br - QPointF(0, RESIZE_HANDLE_SIZE * 0.7));
        painter->drawLine(br - QPointF(RESIZE_HANDLE_SIZE * 0.4, 0), br - QPointF(0, RESIZE_HANDLE_SIZE * 0.4));
    }
}

void CommentGraphicsObject::setGroupedNodes(std::unordered_set<NodeId> const &nodeIds)
{
    _groupedNodeIds = nodeIds;
    adjustBounds();
}

void CommentGraphicsObject::addNode(NodeId nodeId)
{
    // Check if this node already belongs to another comment
    for (auto const& [commentId, comment] : _scene.comments()) {
        if (comment && commentId != _uid && comment->containsNode(nodeId)) {
            return;
        }
    }
    
    // Don't add nodes that belong to child comments
    if (!isNodeInChildComment(nodeId) && _groupedNodeIds.insert(nodeId).second) {
        Q_EMIT nodeAddedToGroup(nodeId);
        adjustBounds();
    }
}

void CommentGraphicsObject::removeNode(NodeId nodeId)
{
    if (_groupedNodeIds.erase(nodeId) > 0) {
        Q_EMIT nodeRemovedFromGroup(nodeId);
        adjustBounds();
    }
}

bool CommentGraphicsObject::containsNode(NodeId nodeId) const
{
    return _groupedNodeIds.find(nodeId) != _groupedNodeIds.end();
}

void CommentGraphicsObject::addChildComment(QUuid const &commentId)
{
    if (_childCommentIds.insert(commentId).second) {
        // When adding a child, remove any of our nodes that are inside the child's bounds
        auto childComment = _scene.commentGraphicsObject(commentId);
        if (childComment) {
            std::unordered_set<NodeId> toRemove;
            QRectF childRect = childComment->sceneBoundingRect();
            
            for (NodeId nodeId : _groupedNodeIds) {
                auto nodeGO = _scene.nodeGraphicsObject(nodeId);
                if (nodeGO) {
                    QRectF nodeRect = nodeGO->sceneBoundingRect();
                    if (childRect.contains(nodeRect)) {
                        toRemove.insert(nodeId);
                    }
                }
            }
            
            for (NodeId nodeId : toRemove) {
                _groupedNodeIds.erase(nodeId);
            }
        }
    }
}

void CommentGraphicsObject::removeChildComment(QUuid const &commentId)
{
    if (_childCommentIds.erase(commentId) > 0) {
        // When a child is removed, we MUST clean up any nodes that might
        // have accidentally ended up in our list that belong to that child
        auto childComment = _scene.commentGraphicsObject(commentId);
        if (childComment) {
            std::unordered_set<NodeId> toRemove;
            for (NodeId nodeId : _groupedNodeIds) {
                // Check if this node is inside the child comment's bounds
                auto nodeGO = _scene.nodeGraphicsObject(nodeId);
                if (nodeGO && childComment) {
                    QRectF nodeRect = nodeGO->sceneBoundingRect();
                    QRectF childRect = childComment->sceneBoundingRect();
                    if (childRect.contains(nodeRect)) {
                        toRemove.insert(nodeId);
                    }
                }
            }
            for (NodeId nodeId : toRemove) {
                _groupedNodeIds.erase(nodeId);
            }
        }
    }
}

bool CommentGraphicsObject::containsChildComment(QUuid const &commentId) const
{
    return _childCommentIds.find(commentId) != _childCommentIds.end();
}

void CommentGraphicsObject::setCommentText(QString const &text)
{
    _textItem->setPlainText(text);
}

QString CommentGraphicsObject::commentText() const
{
    return _textItem->toPlainText();
}

void CommentGraphicsObject::adjustBounds()
{
    if (_groupedNodeIds.empty() && _childCommentIds.empty()) {
        return;
    }

    // Calculate bounding box of all grouped nodes AND child comments
    QRectF boundingBox;
    bool first = true;

    // Include all directly grouped nodes
    for (NodeId nodeId : _groupedNodeIds) {
        auto nodeGO = _scene.nodeGraphicsObject(nodeId);
        if (nodeGO) {
            QRectF nodeRect = nodeGO->sceneBoundingRect();
            if (first) {
                boundingBox = nodeRect;
                first = false;
            } else {
                boundingBox = boundingBox.united(nodeRect);
            }
        }
    }

    // Include all child comments
    for (QUuid const &childId : _childCommentIds) {
        auto childComment = _scene.commentGraphicsObject(childId);
        if (childComment) {
            QRectF childRect = childComment->sceneBoundingRect();
            if (first) {
                boundingBox = childRect;
                first = false;
            } else {
                boundingBox = boundingBox.united(childRect);
            }
        }
    }

    if (!first) {
        // Add padding
        const int padding = 20;
        boundingBox.adjust(-padding, -padding - HEADER_HEIGHT, padding, padding);

        // Update position and size
        prepareGeometryChange();
        setPos(boundingBox.topLeft());
        _rect = QRectF(0, 0, boundingBox.width(), boundingBox.height());
        
        // Ensure minimum size
        if (_rect.width() < MIN_WIDTH) {
            _rect.setWidth(MIN_WIDTH);
        }
        if (_rect.height() < MIN_HEIGHT) {
            _rect.setHeight(MIN_HEIGHT);
        }
    }
}

void CommentGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QPointF pos = event->pos();
        
        if (isResizing(pos)) {
            _isResizing = true;
            _resizeStartPos = pos;
            _resizeStartRect = _rect;
            setCursor(cursorForPosition(pos));
            event->accept();
            return;
        } else if (pos.y() <= HEADER_HEIGHT) {
            _isDragging = true;
            _dragStartPos = event->scenePos();
            setCursor(Qt::ClosedHandCursor);
            event->accept();
            return;
        }
    }
    
    QGraphicsObject::mousePressEvent(event);
}

void CommentGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    if (_isResizing) {
        prepareGeometryChange();
        QPointF delta = event->pos() - _resizeStartPos;
        _rect = _resizeStartRect;
        _rect.setWidth(qMax(static_cast<qreal>(MIN_WIDTH), _resizeStartRect.width() + delta.x()));
        _rect.setHeight(qMax(static_cast<qreal>(MIN_HEIGHT), _resizeStartRect.height() + delta.y()));
        
        // Check for nodes that should be added to the group when resizing
        QRectF commentSceneRect = mapRectToScene(_rect);
        auto allNodeIds = _scene.graphModel().allNodeIds();
        for (NodeId nodeId : allNodeIds) {
            auto nodeGO = _scene.nodeGraphicsObject(nodeId);
            if (nodeGO) {
                QRectF nodeRect = nodeGO->sceneBoundingRect();
                bool isInside = commentSceneRect.contains(nodeRect);
                bool isGrouped = containsNode(nodeId);
                bool inChildComment = isNodeInChildComment(nodeId);
                
                if (isInside && !isGrouped && !inChildComment) {
                    // Additional check: make sure this node isn't in ANY comment at all
                    bool nodeInAnyComment = false;
                    for (auto const& [otherId, otherComment] : _scene.comments()) {
                        if (otherComment && otherComment->containsNode(nodeId)) {
                            nodeInAnyComment = true;
                            break;
                        }
                    }
                    
                    if (!nodeInAnyComment) {
                        // Node is now inside the comment box after resize - add it
                        // Only if it doesn't belong to any comment
                        addNode(nodeId);
                    }
                }
            }
        }
        
        event->accept();
        return;
    } else if (_isDragging) {
        QPointF delta = event->scenePos() - _dragStartPos;
        setPos(pos() + delta);
        updateGroupedNodesPosition(delta);
        updateChildCommentsPosition(delta);
        _dragStartPos = event->scenePos();
        event->accept();
        return;
    }
    
    QGraphicsObject::mouseMoveEvent(event);
}

void CommentGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    _isResizing = false;
    _isDragging = false;
    setCursor(Qt::ArrowCursor);
    
    QGraphicsObject::mouseReleaseEvent(event);
}

void CommentGraphicsObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->pos().y() <= HEADER_HEIGHT) {
        _textItem->setFocus();
        _textItem->setTextInteractionFlags(Qt::TextEditorInteraction);
        event->accept();
        return;
    }
    
    QGraphicsObject::mouseDoubleClickEvent(event);
}

void CommentGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(cursorForPosition(event->pos()));
    QGraphicsObject::hoverEnterEvent(event);
}

void CommentGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(Qt::ArrowCursor);
    QGraphicsObject::hoverLeaveEvent(event);
}

void CommentGraphicsObject::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
    setCursor(cursorForPosition(event->pos()));
    QGraphicsObject::hoverMoveEvent(event);
}

QVariant CommentGraphicsObject::itemChange(GraphicsItemChange change, QVariant const &value)
{
    if (change == ItemPositionHasChanged && scene()) {
        // Check if this comment has moved in/out of another comment
        QRectF myRect = sceneBoundingRect();
        
        // Check all other comments
        for (auto const& [otherId, otherComment] : _scene.comments()) {
            if (otherComment && otherId != _uid) {
                QRectF otherRect = otherComment->sceneBoundingRect();
                bool isInside = otherRect.contains(myRect);
                bool isChild = otherComment->containsChildComment(_uid);
                
                if (isInside && !isChild && otherComment.get() != this) {
                    // This comment moved into another comment
                    if (!_parentCommentId.isNull()) {
                        // Remove from old parent
                        auto oldParent = _scene.commentGraphicsObject(_parentCommentId);
                        if (oldParent) {
                            oldParent->removeChildComment(_uid);
                        }
                    }
                    otherComment->addChildComment(_uid);
                    _parentCommentId = otherId;
                } else if (!isInside && isChild) {
                    // This comment moved out of its parent
                    otherComment->removeChildComment(_uid);
                    if (_parentCommentId == otherId) {
                        _parentCommentId = QUuid();
                    }
                }
            }
        }
    }
    
    return QGraphicsObject::itemChange(change, value);
}

void CommentGraphicsObject::updateGroupedNodesPosition(QPointF const &delta)
{
    // Move all nodes that are directly owned by this comment
    // Parent comments should never have child comment nodes in their list
    for (NodeId nodeId : _groupedNodeIds) {
        auto nodeGO = _scene.nodeGraphicsObject(nodeId);
        if (nodeGO) {
            nodeGO->setPos(nodeGO->pos() + delta);
            _scene.graphModel().setNodeData(nodeId, NodeRole::Position, nodeGO->pos());
        }
    }
}

void CommentGraphicsObject::updateChildCommentsPosition(QPointF const &delta)
{
    for (QUuid const &childId : _childCommentIds) {
        auto childComment = _scene.commentGraphicsObject(childId);
        if (childComment) {
            childComment->setPos(childComment->pos() + delta);
            // Recursively update the child's nodes and its children
            childComment->updateGroupedNodesPosition(delta);
            childComment->updateChildCommentsPosition(delta);
        }
    }
}

void CommentGraphicsObject::calculateBoundingRect()
{
    // Already implemented in adjustBounds()
}

bool CommentGraphicsObject::isResizing(QPointF const &pos) const
{
    QRectF resizeRect(
        _rect.right() - RESIZE_HANDLE_SIZE,
        _rect.bottom() - RESIZE_HANDLE_SIZE,
        RESIZE_HANDLE_SIZE,
        RESIZE_HANDLE_SIZE
    );
    return resizeRect.contains(pos);
}

Qt::CursorShape CommentGraphicsObject::cursorForPosition(QPointF const &pos) const
{
    if (isResizing(pos)) {
        return Qt::SizeFDiagCursor;
    } else if (pos.y() <= HEADER_HEIGHT) {
        return Qt::OpenHandCursor;
    }
    return Qt::ArrowCursor;
}

bool CommentGraphicsObject::isNodeInChildComment(NodeId nodeId) const
{
    // Check if this node belongs to any of our child comments
    for (QUuid const &childId : _childCommentIds) {
        auto childComment = _scene.commentGraphicsObject(childId);
        if (childComment && childComment->containsNode(nodeId)) {
            return true;
        }
    }
    return false;
}

void CommentGraphicsObject::cleanupNodeOwnership()
{
    // Remove any nodes from our list that belong to child comments
    std::unordered_set<NodeId> nodesToRemove;
    for (NodeId nodeId : _groupedNodeIds) {
        if (isNodeInChildComment(nodeId)) {
            nodesToRemove.insert(nodeId);
        }
    }
    
    for (NodeId nodeId : nodesToRemove) {
        _groupedNodeIds.erase(nodeId);
    }
}

} // namespace QtNodes