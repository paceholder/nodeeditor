#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsView>

#include "FlowScene.hpp"

#include "Connection.hpp"
#include "ConnectionGeometry.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionState.hpp"
#include "ConnectionBlurEffect.hpp"

#include "NodeGraphicsObject.hpp"

#include "NodeConnectionInteraction.hpp"

#include "Node.hpp"

ConnectionGraphicsObject::
ConnectionGraphicsObject(FlowScene &scene,
                         std::shared_ptr<Connection> &connection)
  : _scene(scene)
  , _connection(connection)
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  setAcceptHoverEvents(true);

  // addGraphicsEffect();

  setZValue(-1.0);
}


ConnectionGraphicsObject::
~ConnectionGraphicsObject()
{
  std::cout << "Remove ConnectionGraphicsObject from scene" << std::endl;

  _scene.removeItem(this);
}


QRectF
ConnectionGraphicsObject::
boundingRect() const
{
  return _connection.lock()->connectionGeometry().boundingRect();
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

  return ConnectionPainter::getPainterStroke(_connection.lock()->connectionGeometry());

#endif
}


void
ConnectionGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}


void
ConnectionGraphicsObject::
paint(QPainter* painter,
      QStyleOptionGraphicsItem const* option,
      QWidget*)
{
  painter->setClipRect(option->exposedRect);

  ConnectionPainter::paint(painter,
                           _connection.lock()->connectionGeometry(),
                           _connection.lock()->connectionState());
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

  auto node = ::locateNodeAt(event->scenePos(),
                             _scene,
                             _scene.views()[0]->transform());

  auto &state = _connection.lock()->connectionState();
  state.interactWithNode(node, event->scenePos());

  //-------------------

  QPointF offset = event->pos() - event->lastPos();

  auto requiredPort = _connection.lock()->requiredPort();

  if (requiredPort != PortType::NONE)
  {
    _connection.lock()->connectionGeometry().moveEndPoint(requiredPort,
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

  //std::shared_ptr<Node> node = ::locateNodeAt(event);
  auto node = ::locateNodeAt(event->scenePos(), _scene,
                             _scene.views()[0]->transform());

  if (node)
  {
    NodeConnectionInteraction interaction(node, _connection.lock());

    if (interaction.tryConnect())
    {
      node->resetReactionToConnection();
    }
    else
    {
      _scene.deleteConnection(_connection.lock()->id());
    }
  }
  else
  {
    _scene.deleteConnection(_connection.lock()->id());
  }
}


void
ConnectionGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  _connection.lock()->connectionGeometry().setHovered(true);

  update();
  event->accept();
}


void
ConnectionGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  _connection.lock()->connectionGeometry().setHovered(false);

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
