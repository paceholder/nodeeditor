#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "NodeState.hpp"

class QGraphicsProxyWidget;

namespace QtNodes {

class BasicGraphicsScene;
class AbstractGraphModel;

class NodeGraphicsObject : public QGraphicsObject
{
    Q_OBJECT
public:
    // Needed for qgraphicsitem_cast
    enum { Type = UserType + 1 };

    int type() const override { return Type; }

public:
    NodeGraphicsObject(BasicGraphicsScene &scene, NodeId node);

    ~NodeGraphicsObject() override = default;

public:
    AbstractGraphModel &graphModel() const;

    BasicGraphicsScene *nodeScene() const;

    NodeId nodeId() { return _nodeId; }

    NodeId nodeId() const { return _nodeId; }

    NodeState &nodeState() { return _nodeState; }

    NodeState const &nodeState() const { return _nodeState; }

    QRectF boundingRect() const override;

    void setGeometryChanged();

    /// Visits all attached connections and corrects
    /// their corresponding end points.
    void moveConnections() const;

    /// Repaints the node once with reacting ports.
    void reactToConnection(ConnectionGraphicsObject const *cgo);

protected:
    void paint(QPainter *painter,
               QStyleOptionGraphicsItem const *option,
               QWidget *widget = 0) override;

    QVariant itemChange(GraphicsItemChange change, const QVariant &value) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

private:
    void embedQWidget();

    void setLockedState();

private:
    NodeId _nodeId;

    AbstractGraphModel &_graphModel;

    NodeState _nodeState;

    // either nullptr or owned by parent QGraphicsItem
    QGraphicsProxyWidget *_proxyWidget;
};
} // namespace QtNodes
