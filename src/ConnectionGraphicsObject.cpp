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
    [&](PortType port)
    {
      auto address = _connection.getAddress(port);

      if (address.first == id)
      {
        auto& p = _connection.connectionGeometry().getEndPoint(port);

        _connection.connectionGeometry().setEndPoint(port, p + offset);
      }
    };

  moveEndPoint(PortType::IN);
  moveEndPoint(PortType::OUT);

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

  auto &state = _connection.connectionState();
  state.interactWithNode(node, event->scenePos());

  //-------------------

  QPointF offset       = event->pos() - event->lastPos();

  auto    requiredPort = _connection.requiredPort();

  if (requiredPort != PortType::NONE)
  {
    _connection.connectionGeometry().moveEndPoint(requiredPort,
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
