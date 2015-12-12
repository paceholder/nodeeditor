#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QStyleOptionGraphicsItem>

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
  setCacheMode( QGraphicsItem::DeviceCoordinateCache );

  setAcceptHoverEvents(true);

  //{
  //auto effect = new QGraphicsDropShadowEffect;
  //effect->setOffset(4, 4);
  //effect->setBlurRadius(20);
  //effect->setColor(QColor(Qt::gray).darker(800));
  //setGraphicsEffect(effect);
  //}

  FlowScene &flowScene = FlowScene::instance();
  flowScene.addItem(this);
}


QRectF
ConnectionGraphicsObject::
boundingRect() const
{
  return _connectionGeometry.boundingRect();
}


void
ConnectionGraphicsObject::
onItemMoved(QUuid id, QPointF const &offset)
{
  prepareGeometryChange();

  auto moveEndPoint =
    [&](EndType end)
    {
      auto address = _connection.getAddress(end);

      if (address.first == id)
      {

        auto p = _connectionGeometry.getEndPoint(end);

        _connectionGeometry.setEndPoint(end, p + offset);
      }
    };

  moveEndPoint(EndType::SINK);
  moveEndPoint(EndType::SOURCE);
}


QPainterPath
ConnectionGraphicsObject::
shape() const
{
#ifdef DEBUG_DRAWING

  //QPainterPath path;

  //path.addRect(boundingRect());
  //return path;

#else

  return _connectionPainter.getPainterStroke();

#endif
}


void
ConnectionGraphicsObject::
paint(QPainter* painter, QStyleOptionGraphicsItem const* option, QWidget*)
{

  painter->setClipRect(option->exposedRect);
  _connectionPainter.paint(painter);
}


void
ConnectionGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  event->ignore();
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


void
ConnectionGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  _connectionGeometry.setHovered(true);

  update();
  event->accept();
}


void
ConnectionGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  _connectionGeometry.setHovered(false);

  update();
  event->accept();
}
