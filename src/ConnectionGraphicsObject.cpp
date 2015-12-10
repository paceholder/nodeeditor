#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsSceneMouseEvent>

#include "FlowScene.hpp"

#include "Connection.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionPainter.hpp"

ConnectionGraphicsObject::
ConnectionGraphicsObject(Connection& connection,
                         ConnectionGeometry& connectionGeometry,
                         ConnectionPainter const& connectionPainter)
  : _connection(connection)
  , _connectionGeometry(connectionGeometry)
  , _connectionPainter(connectionPainter)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  {
    auto effect = new QGraphicsDropShadowEffect;
    effect->setOffset(4, 4);
    effect->setBlurRadius(20);
    effect->setColor(QColor(Qt::gray).darker(800));
    setGraphicsEffect(effect);
  }

  grabMouse();
}


QRectF
ConnectionGraphicsObject::
boundingRect() const
{
  return _connectionGeometry.boundingRect();
}


void
ConnectionGraphicsObject::
paint(QPainter* painter, QStyleOptionGraphicsItem const*, QWidget*)
{
  _connectionPainter.paint(painter);
}


void
ConnectionGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QPointF p = event->pos() - event->lastPos();

  prepareGeometryChange();

  auto draggingEnd = _connection.draggingEnd();

  if (draggingEnd != EndType::NONE)
  {
    auto &endPoint = _connectionGeometry.getEndPoint(draggingEnd);

    endPoint += p;
  }

  event->accept();
}


void
ConnectionGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  (void)event;

  auto node = FlowScene::locateNodeAt(event);

  if (node)
  {
    _connection.tryConnectToNode(node, event->scenePos());
  }

  ungrabMouse();

  event->accept();
}
