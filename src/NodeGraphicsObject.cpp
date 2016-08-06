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

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  setCacheMode( QGraphicsItem::DeviceCoordinateCache );

  {
    auto effect = new QGraphicsDropShadowEffect;
    effect->setOffset(4, 4);
    effect->setBlurRadius(20);
    effect->setColor(QColor(Qt::gray).darker(800));

    setGraphicsEffect(effect);

    setOpacity(_node.lock()->nodeGeometry().opacity());
  }

  setAcceptHoverEvents(true);

  embedQWidget();
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

  QWidget * w = node->nodeDataModel()->embeddedWidget();

  if (w)
  {
    _proxyWidget = new QGraphicsProxyWidget();

    _proxyWidget->setWidget(w);

    w->setVisible(true);
    w->setMaximumSize(w->sizeHint());

    _proxyWidget->setParentItem(this);
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
moveConnections() const
{
  std::shared_ptr<Node> node = _node.lock();

  NodeState const & nodeState = node->nodeState();

  auto moveConnections =
    [&](PortType portType)
    {
      auto const & connectionsWeak = nodeState.getEntries(portType);

      size_t portIndex = 0;

      for (auto const & connection : connectionsWeak)
      {
        QPointF scenePos =
          node->nodeGeometry().portScenePosition(portIndex,
                                                 portType,
                                                 sceneTransform());

        auto con = connection.lock();

        if (con)
        {
          QTransform sceneTransform =
            con->getConnectionGraphicsObject()->sceneTransform();

          QPointF connectionPos = sceneTransform.inverted().map(scenePos);

          con->connectionGeometry().setEndPoint(portType,
                                                connectionPos);

          con->getConnectionGraphicsObject()->setGeometryChanged();
          con->getConnectionGraphicsObject()->update();
        }

        ++portIndex;
      }
    };

  moveConnections(PortType::IN);

  moveConnections(PortType::OUT);
}


void
NodeGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* )
{
  painter->setClipRect(option->exposedRect);

  auto node = _node.lock();

  NodeGeometry & geom = node->nodeGeometry();

  if (_proxyWidget)
  {
    _proxyWidget->setPos(geom.widgetPosition());
  }

  NodePainter::paint(painter, node);
}


void
NodeGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent * event)
{
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

  clickPort(PortType::IN);
  clickPort(PortType::OUT);

  event->accept();
}


void
NodeGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsObject::mouseMoveEvent(event);

  //QPointF d = event->pos() - event->lastPos();

  if (event->lastPos() != event->pos())
    //moveConnections(d);
    moveConnections();

  //event->accept();
}


void
NodeGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
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
