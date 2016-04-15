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
ConnectionGraphicsObject(Connection& connection)
  : _connection(connection)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  setAcceptHoverEvents(true);

  {
    //auto effect = new QGraphicsDropShadowEffect;
    //auto effect = new QGraphicsBlurEffect;
    //auto effect = new ConnectionBlurEffect(this);

    //effect->setOffset(4, 4);
    //effect->setBlurRadius(5);
    //effect->setColor(QColor(Qt::gray).darker(800));
    //setGraphicsEffect(effect);
  }

  FlowScene &flowScene = FlowScene::instance();
  flowScene.addItem(this);
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

  auto node = FlowScene::locateNodeAt(event);

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
    auto &endPoint = _connection.connectionGeometry().getEndPoint(draggingEnd);

    _connection.connectionGeometry().setEndPoint(draggingEnd,
                                                 endPoint + offset);
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

  auto node = FlowScene::locateNodeAt(event);

  if (node && _connection.tryConnectToNode(node, event->scenePos()))
  {
    node->resetReactionToConnection();
  }
  else
  {
    auto& scene = FlowScene::instance();
    scene.deleteConnection(&_connection);
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
