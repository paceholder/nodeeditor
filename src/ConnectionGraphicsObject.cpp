#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QStyleOptionGraphicsItem>

#include "FlowScene.hpp"

#include "Connection.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionState.hpp"

#include "ConnectionBlurEffect.hpp"

#include "Node.hpp"

ConnectionGraphicsObject::
ConnectionGraphicsObject(Connection &connection)
  : _connection(connection)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  setAcceptHoverEvents(true);

  // addGraphicsEffect();

  FlowScene::instance().addItem(this);
}


QRectF
ConnectionGraphicsObject::
boundingRect() const
{
  return _connection.connectionGeometry().boundingRect();
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
        auto& p = _connection.connectionGeometry().getEndPoint(end);

        _connection.connectionGeometry().setEndPoint(end, p + offset);
      }
    };

  moveEndPoint(EndType::SINK);
  moveEndPoint(EndType::SOURCE);

  update();
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

  return ConnectionPainter::getPainterStroke(_connection.connectionGeometry());

#endif
}


void
ConnectionGraphicsObject::
paint(QPainter* painter,
      QStyleOptionGraphicsItem const* option,
      QWidget*)
{
  painter->setClipRect(option->exposedRect);

  ConnectionPainter::paint(painter,
                           _connection.connectionGeometry(),
                           _connection.connectionState());
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
  prepareGeometryChange();

  //-------------------

  auto node = ::locateNodeAt(event);

  if (node)
  {
    node->reactToPossibleConnection(_connection.draggingEnd(),
                                    event->scenePos());

    auto &state = _connection.connectionState();
    state.setLastHoveredNode(node->id());
  }
  else
  {
    auto &state = _connection.connectionState();

    state.resetLastHoveredNode();
  }

  //-------------------

  QPointF offset      = event->pos() - event->lastPos();
  auto    draggingEnd = _connection.draggingEnd();

  if (draggingEnd != EndType::NONE)
  {
    _connection.connectionGeometry().moveEndPoint(draggingEnd,
                                                  offset);
  }

  //-------------------

  update();

  event->accept();
}


void
ConnectionGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  ungrabMouse();
  event->accept();

  //---------------

  std::shared_ptr<Node> node = ::locateNodeAt(event);

  if (node && _connection.tryConnectToNode(node, event->scenePos()))
  {
    node->resetReactionToConnection();
  }
  else
  {
    FlowScene::instance().deleteConnection(_connection.id());
  }
}


void
ConnectionGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  _connection.connectionGeometry().setHovered(true);

  update();
  event->accept();
}


void
ConnectionGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  _connection.connectionGeometry().setHovered(false);

  update();
  event->accept();
}


void
ConnectionGraphicsObject::
addGraphicsEffect()
{
  auto effect = new QGraphicsBlurEffect;

  effect->setBlurRadius(5);
  setGraphicsEffect(effect);

  //auto effect = new QGraphicsDropShadowEffect;
  //auto effect = new ConnectionBlurEffect(this);
  //effect->setOffset(4, 4);
  //effect->setColor(QColor(Qt::gray).darker(800));
}
