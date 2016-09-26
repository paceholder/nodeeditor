#include "NodeGraphicsObject.hpp"

#include <iostream>
#include <cstdlib>

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

#include "FlowScene.hpp"
#include "NodePainter.hpp"

#include "Node.hpp"
#include "NodeDataModel.hpp"
#include "NodeConnectionInteraction.hpp"

NodeGraphicsObject::
NodeGraphicsObject(FlowScene &scene,
                   std::shared_ptr<Node>& node)
  : _scene(scene)
  , _node(node)
  , _proxyWidget(nullptr)
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges);

  setCacheMode( QGraphicsItem::DeviceCoordinateCache );

  {
    auto effect = new QGraphicsDropShadowEffect;
    effect->setOffset(4, 4);
    effect->setBlurRadius(20);
    effect->setColor(QColor(Qt::gray).darker(800));

    setGraphicsEffect(effect);
  }

  setOpacity(_node.lock()->nodeGeometry().opacity());

  setAcceptHoverEvents(true);

  embedQWidget();
}


NodeGraphicsObject::
~NodeGraphicsObject()
{
  _scene.removeItem(this);
}


std::weak_ptr<Node>&
NodeGraphicsObject::
node()
{
  return _node;
}


void
NodeGraphicsObject::
embedQWidget()
{
  auto node = _node.lock();
  NodeGeometry & geom = node->nodeGeometry();

  if (auto w = node->nodeDataModel()->embeddedWidget())
  {
    _proxyWidget = new QGraphicsProxyWidget(this);

    _proxyWidget->setWidget(w);

    _proxyWidget->setPreferredWidth(5);

    geom.recalculateSize();

    _proxyWidget->setPos(geom.widgetPosition());

    update();

    _proxyWidget->setOpacity(1.0);
    _proxyWidget->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
  }
}


QRectF
NodeGraphicsObject::
boundingRect() const
{
  return _node.lock()->nodeGeometry().boundingRect();
}


void
NodeGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}


void
NodeGraphicsObject::
moveConnections() const
{
  std::shared_ptr<Node> node = _node.lock();

  NodeState const & nodeState = node->nodeState();

  auto moveConnections =
  [&](PortType portType)
  {
    auto const & connectionsWeak = nodeState.getEntries(portType);

    for (auto const & connection : connectionsWeak)
    {
      if (auto con = connection.lock())
        con->getConnectionGraphicsObject()->move();
    }
  };

  moveConnections(PortType::In);

  moveConnections(PortType::Out);
}


void
NodeGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* )
{
  painter->setClipRect(option->exposedRect);

  auto node = _node.lock();

  NodePainter::paint(painter, node);
}


QVariant
NodeGraphicsObject::
itemChange(GraphicsItemChange change, const QVariant &value)
{
  if (change == ItemPositionChange && scene())
  {
    moveConnections();
  }

  return QGraphicsItem::itemChange(change, value);
}


void
NodeGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent * event)
{

  if (!this->isSelected() && !(event->modifiers() & Qt::ControlModifier))
  {
    _scene.clearSelection();
  }

  auto clickPort =
  [&](PortType portToCheck)
  {
    auto node = _node.lock();

    NodeGeometry & nodeGeometry = node->nodeGeometry();

    // TODO do not pass sceneTransform
    int portIndex = nodeGeometry.checkHitScenePoint(portToCheck,
                                                    event->scenePos(),
                                                    sceneTransform());

    if (portIndex != INVALID)
    {
      NodeState const & nodeState = node->nodeState();

      std::shared_ptr<Connection> connection =
        nodeState.connection(portToCheck, portIndex);

      // start dragging existing connection
      if (connection)
      {
        NodeConnectionInteraction interaction(node, connection);

        interaction.disconnect(portToCheck);
      }
      // initialize new Connection
      else
      {
        // todo add to FlowScene
        auto connection = _scene.createConnection(portToCheck,
                                                  node,
                                                  portIndex);

        node->nodeState().setConnection(portToCheck,
                                        portIndex,
                                        connection);

        connection->getConnectionGraphicsObject()->grabMouse();
      }
    }
  };

  clickPort(PortType::In);
  clickPort(PortType::Out);

  auto pos     = event->pos();
  auto node    = _node.lock();
  auto & geom  = node->nodeGeometry();
  auto & state = node->nodeState();

  if (geom.resizeRect().contains(QPoint(pos.x(),
                                        pos.y())))
  {
    state.setResizing(true);
  }
}


void
NodeGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  auto node    = _node.lock();
  auto & geom  = node->nodeGeometry();
  auto & state = node->nodeState();

  if (state.resizing())
  {
    auto diff = event->pos() - event->lastPos();

    if (auto w = node->nodeDataModel()->embeddedWidget())
    {
      prepareGeometryChange();

      auto oldSize = w->size();

      oldSize += QSize(diff.x(), diff.y());

      w->setFixedSize(oldSize);

      _proxyWidget->setMinimumSize(oldSize);
      _proxyWidget->setMaximumSize(oldSize);
      _proxyWidget->setPos(geom.widgetPosition());

      geom.recalculateSize();
      update();

      moveConnections();

      event->accept();
    }
  }
  else
  {
    QGraphicsObject::mouseMoveEvent(event);

    if (event->lastPos() != event->pos())
      moveConnections();

    event->ignore();
  }

  QRectF r = scene()->sceneRect();

  r = r.united(mapToScene(boundingRect()).boundingRect());

  scene()->setSceneRect(r);
}


void
NodeGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  auto node    = _node.lock();
  auto & state = node->nodeState();

  state.setResizing(false);

  QGraphicsObject::mouseReleaseEvent(event);

  // position connections precisely after fast node move
  moveConnections();
}


void
NodeGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  _node.lock()->nodeGeometry().setHovered(true);
  update();
  event->accept();
}


void
NodeGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  _node.lock()->nodeGeometry().setHovered(false);
  update();
  event->accept();
}


void
NodeGraphicsObject::
hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  auto pos    = event->pos();
  auto node   = _node.lock();
  auto & geom = node->nodeGeometry();

  if (geom.resizeRect().contains(QPoint(pos.x(),
                                        pos.y())))
  {
    setCursor(QCursor(Qt::SizeFDiagCursor));
  }
  else
  {
    setCursor(QCursor());
  }

  event->accept();
}
