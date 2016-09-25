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
move()
{
  auto con = _connection.lock();

  if (!con)
    return;

  auto moveEndPoint =
  [&con, this] (PortType portType)
  {
    if (auto node = con->getNode(portType).lock())
    {
      auto const &nodeGraphics = node->nodeGraphicsObject();

      auto const &nodeGeom = node->nodeGeometry();

      QPointF scenePos =
        nodeGeom.portScenePosition(con->getPortIndex(portType),
                                   portType,
                                   nodeGraphics->sceneTransform());

      {
        QTransform sceneTransform = this->sceneTransform();

        QPointF connectionPos = sceneTransform.inverted().map(scenePos);

        con->connectionGeometry().setEndPoint(portType,
                                              connectionPos);

        con->getConnectionGraphicsObject()->setGeometryChanged();
        con->getConnectionGraphicsObject()->update();
      }
    }
  };

  moveEndPoint(PortType::In);
  moveEndPoint(PortType::Out);
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

  auto view = static_cast<QGraphicsView*>(event->widget());
  auto node = ::locateNodeAt(event->scenePos(),
                             _scene,
                             view->transform());

  auto con    = _connection.lock();
  auto &state = con->connectionState();

  state.interactWithNode(node);
  if (node)
  {
    node->reactToPossibleConnection(state.requiredPort(),
                                    con->dataType(),
                                    event->scenePos());
  }

  //-------------------

  QPointF offset = event->pos() - event->lastPos();

  auto requiredPort = con->requiredPort();

  if (requiredPort != PortType::None)
  {
    con->connectionGeometry().moveEndPoint(requiredPort, offset);
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

  auto node = ::locateNodeAt(event->scenePos(), _scene,
                             _scene.views()[0]->transform());

  auto connection = _connection.lock();

  NodeConnectionInteraction interaction(node, connection);

  if (node && interaction.tryConnect())
  {
    node->resetReactionToConnection();
  }
  else
  {
    _scene.deleteConnection(connection);
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
