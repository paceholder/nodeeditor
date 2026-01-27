#pragma once

#include "NodeDelegateModel.hpp"
#include "NodeGroup.hpp"
#include "NodeState.hpp"
#include <QIcon>
#include <QRectF>
#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

class QGraphicsProxyWidget;

namespace QtNodes {

class BasicGraphicsScene;
class AbstractGraphModel;
class NodeGroup;
class NodeDelegateModel;
class GroupGraphicsObject;

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

    /// Lockes/unlockes nodes in a selected node group.
    void lock(bool locked);

    void updateQWidgetEmbedPos();

    /// Saves node in a QJsonObject save file.
    QJsonObject save() const;

    /** @brief Setter for the NodeGroup object.
     *  @param shared pointer to the node group.
     */
    void setNodeGroup(std::shared_ptr<NodeGroup> group);

    /// Unsets NodeGroup, setting it to an empty pointer.
    void unsetNodeGroup() { _nodeGroup = std::weak_ptr<NodeGroup>(); }

    /// Getter for the NodeGroup object.
    std::weak_ptr<NodeGroup> nodeGroup() const { return _nodeGroup; }

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

    bool _locked;

    bool _draggingIntoGroup;
    GroupGraphicsObject *_possibleGroup;
    QRectF _originalGroupSize;

    // either nullptr or owned by parent QGraphicsItem
    QGraphicsProxyWidget *_proxyWidget;

    std::weak_ptr<NodeGroup> _nodeGroup{};
};
} // namespace QtNodes
