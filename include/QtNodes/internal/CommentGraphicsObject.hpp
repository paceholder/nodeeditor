#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsTextItem>
#include <unordered_set>

#include "Definitions.hpp"
#include "Export.hpp"
#include "QUuidStdHash.hpp"

namespace QtNodes {

class BasicGraphicsScene;
class NodeGraphicsObject;

class NODE_EDITOR_PUBLIC CommentGraphicsObject : public QGraphicsObject
{
    Q_OBJECT

public:
    CommentGraphicsObject(BasicGraphicsScene &scene,
                          QUuid const &uid,
                          QGraphicsItem *parent = nullptr);

    ~CommentGraphicsObject() override;

    QUuid const &id() const { return _uid; }

    QRectF boundingRect() const override;

    void paint(QPainter *painter,
               QStyleOptionGraphicsItem const *option,
               QWidget *widget = nullptr) override;

    void setGroupedNodes(std::unordered_set<NodeId> const &nodeIds);
    std::unordered_set<NodeId> const &groupedNodes() const { return _groupedNodeIds; }

    void addNode(NodeId nodeId);
    void removeNode(NodeId nodeId);
    bool containsNode(NodeId nodeId) const;
    
    void addChildComment(QUuid const &commentId);
    void removeChildComment(QUuid const &commentId);
    bool containsChildComment(QUuid const &commentId) const;
    std::unordered_set<QUuid> const &childComments() const { return _childCommentIds; }
    
    void setParentComment(QUuid const &parentId) { _parentCommentId = parentId; }
    QUuid parentComment() const { return _parentCommentId; }

    void setCommentText(QString const &text);
    QString commentText() const;

    void adjustBounds();

    void setColor(QColor const &color) { _color = color; }
    QColor color() const { return _color; }

Q_SIGNALS:
    void commentTextChanged(QString const &text);
    void nodeAddedToGroup(NodeId nodeId);
    void nodeRemovedFromGroup(NodeId nodeId);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverMoveEvent(QGraphicsSceneHoverEvent *event) override;

    QVariant itemChange(GraphicsItemChange change, QVariant const &value) override;

private:
    void updateGroupedNodesPosition(QPointF const &delta);
    void updateChildCommentsPosition(QPointF const &delta);
    void calculateBoundingRect();
    bool isResizing(QPointF const &pos) const;
    Qt::CursorShape cursorForPosition(QPointF const &pos) const;
    bool isNodeInChildComment(NodeId nodeId) const;
    void cleanupNodeOwnership();

private:
    BasicGraphicsScene &_scene;
    QUuid _uid;
    std::unordered_set<NodeId> _groupedNodeIds;
    std::unordered_set<QUuid> _childCommentIds;
    QUuid _parentCommentId;

    QGraphicsTextItem *_textItem;
    QRectF _rect;
    QColor _color;

    bool _isResizing;
    bool _isDragging;
    QPointF _resizeStartPos;
    QRectF _resizeStartRect;
    QPointF _dragStartPos;

    static constexpr int RESIZE_HANDLE_SIZE = 10;
    static constexpr int HEADER_HEIGHT = 30;
    static constexpr int MIN_WIDTH = 150;
    static constexpr int MIN_HEIGHT = 100;
};

} // namespace QtNodes