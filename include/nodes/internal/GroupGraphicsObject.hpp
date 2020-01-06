#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"

#include "NodeGeometry.hpp"
#include "NodeState.hpp"

class QGraphicsProxyWidget;

namespace QtNodes
{

class FlowScene;
class FlowItemEntry;

/// Class reacts on GUI events, mouse clicks and
/// forwards painting operation.
class GroupGraphicsObject : public QGraphicsObject
{
  Q_OBJECT

public:
  GroupGraphicsObject(FlowScene &scene);

  virtual
  ~GroupGraphicsObject();

  QRectF
  boundingRect() const override;

  void
  setGeometryChanged();

  enum { Type = UserType + 1 };

  int
  type() const override { return Type; }

  void
  lock(bool locked);

protected:
  void
  paint(QPainter*                       painter,
        QStyleOptionGraphicsItem const* option,
        QWidget*                        widget = 0) override;

  QVariant
  itemChange(GraphicsItemChange change, const QVariant &value) override;

  void
  mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void
  mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void
  mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void
  hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void
  hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  void
  hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

  void
  mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

  void
  contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;


private:
  FlowScene & _scene;
};
}
