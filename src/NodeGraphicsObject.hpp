#ifndef NODE_GRAPHICS_OBJECT_HPP
#define NODE_GRAPHICS_OBJECT_HPP

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"

#include "NodeGeometry.hpp"
#include "NodeState.hpp"

class FlowItemEntry;

class NodeGraphicsObject : public QGraphicsObject
{
  Q_OBJECT

public:
  NodeGraphicsObject(Node& node,
                     NodeState& nodeState,
                     NodeGeometry& nodeGeometry);

  Node* node();

  QRectF boundingRect() const override;

signals:
  void itemMoved(QUuid, QPointF const &);

protected:
  void paint(QPainter*                       painter,
             QStyleOptionGraphicsItem const* option,
             QWidget*                        widget = 0) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  void embedQWidget();

private:
  // addressing

  Node& _node;

  NodeState& _nodeState;

  // painting

  NodeGeometry& _nodeGeometry;
};

#endif // NODE_GRAPHICS_OBJECT_HPP
