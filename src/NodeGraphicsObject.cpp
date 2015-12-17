#include "NodeGraphicsObject.hpp"

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include <iostream>
#include <cstdlib>

#include "ConnectionGraphicsObject.hpp"
#include "FlowItemEntry.hpp"
#include "FlowScene.hpp"
#include "NodePainter.hpp"

NodeGraphicsObject::
NodeGraphicsObject(Node* node,
                   NodeState& nodeState,
                   NodeGeometry& nodeGeometry)
  : _node(node)
  , _nodeState(nodeState)
  , _nodeGeometry(nodeGeometry)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  // TODO: Pass state to geometry

  auto effect = new QGraphicsDropShadowEffect;
  effect->setOffset(4, 4);
  effect->setBlurRadius(20);
  effect->setColor(QColor(Qt::gray).darker(800));

  setGraphicsEffect(effect);

  setOpacity(_nodeGeometry.opacity());
}


void
NodeGraphicsObject::
initializeNode()
{
  setAcceptHoverEvents(true);

  //embedQWidget();

  _nodeGeometry.recalculateSize();
}


void
NodeGraphicsObject::
embedQWidget()
{
  QPushButton* button = new QPushButton(QString("Hello"));

  QGraphicsProxyWidget* proxyWidget = new QGraphicsProxyWidget();

  proxyWidget->setWidget(button);

  button->setVisible(true);
  proxyWidget->setParentItem(this);
}


QRectF
NodeGraphicsObject::
boundingRect() const
{
  return _nodeGeometry.boundingRect();
}


QPointF
NodeGraphicsObject::
connectionPointScenePosition(std::pair<QUuid, int> address,
                             EndType endType) const
{
  return mapToScene(_nodeGeometry.connectionPointScenePosition(address.second,
                                                               endType));
}


QPointF
NodeGraphicsObject::
connectionPointScenePosition(int index,
                             EndType endType) const
{
  return mapToScene(_nodeGeometry.connectionPointScenePosition(index,
                                                               endType));
}


void
NodeGraphicsObject::
reactToPossibleConnection(EndType,
                          QPointF const &scenePoint)
{
  _nodeGeometry.setDraggingPosition(mapFromScene(scenePoint));
}


bool
NodeGraphicsObject::
canConnect(EndType draggingEnd, QPointF const &scenePoint)
{
  int hit = checkHitScenePoint(draggingEnd, scenePoint);

  auto &entries = _nodeState.getEntries(draggingEnd);

  return ((hit >= 0) &&
          _nodeState.connectionID(draggingEnd, hit).isNull());
}


std::pair<QUuid, int>
NodeGraphicsObject::
connect(Connection const* connection,
        EndType draggingEnd,
        int hit)
{
  _nodeState.setConnectionId(draggingEnd, hit, connection->id());

  QObject::connect(this, &NodeGraphicsObject::itemMoved,
                   connection->getConnectionGraphicsObject(),
                   &ConnectionGraphicsObject::onItemMoved);

  connection->getConnectionGraphicsObject()->setZValue(-1.0);

  auto address = std::make_pair(_id, hit);

  return address;
}


std::pair<QUuid, int>
NodeGraphicsObject::
connect(Connection const* connection,
        EndType draggingEnd,
        QPointF const & scenePoint)
{
  int hit = checkHitScenePoint(draggingEnd, scenePoint);

  return connect(connection, draggingEnd, hit);
}


void
NodeGraphicsObject::
disconnect(Connection const* connection,
           EndType endType,
           int hit)
{
  QObject::disconnect(this, &NodeGraphicsObject::itemMoved,
                      connection->getConnectionGraphicsObject(),
                      &ConnectionGraphicsObject::onItemMoved);

  _nodeState.setConnectionId(endType, hit, QUuid());
}


void
NodeGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* )
{
  painter->setClipRect(option->exposedRect);

  NodePainter::paint(painter, _nodeGeometry, _nodeState);
}


// todo make unsigned, define invalid #
int
NodeGraphicsObject::
checkHitScenePoint(EndType endType,
                   QPointF const point) const
{

  switch (endType)
  {
    case EndType::SINK:
      return checkHitSinkScenePoint(point);
      break;

    case EndType::SOURCE:
      return checkHitSourceScenePoint(point);
      break;

    default:
      break;
  }

  return -1;
}


int
NodeGraphicsObject::
checkHitSinkScenePoint(const QPointF eventPoint) const
{
  int result = -1;

  auto   diameter  = _nodeGeometry.connectionPointDiameter();
  double tolerance = 2.0 * diameter;

  for (size_t i = 0; i < _nodeState.getEntries(EndType::SINK).size(); ++i)
  {
    QPointF p = connectionPointScenePosition(i, EndType::SINK) - eventPoint;

    auto distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
      result = i;
  }

  return result;
}


int
NodeGraphicsObject::
checkHitSourceScenePoint(const QPointF eventPoint) const
{
  int result = -1;

  auto   diameter  = _nodeGeometry.connectionPointDiameter();
  double tolerance = 2.0 * diameter;

  for (size_t i = 0; i < _nodeState.getEntries(EndType::SOURCE).size(); ++i)
  {
    QPointF p = connectionPointScenePosition(i, EndType::SOURCE) - eventPoint;
    auto    distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
      result = i;
  }

  return result;
}


void
NodeGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  auto clickEnd =
    [&](EndType endToCheck)
    {
      int hit = checkHitScenePoint(endToCheck,
                                   event->scenePos());

      FlowScene &flowScene = FlowScene::instance();

      std::cout << "HIT " << hit << std::endl;

      if (hit >= 0)
      {
        QUuid const id = _nodeState.connectionID(endToCheck, hit);

        if (id.isNull())
        {
          // todo add to FlowScene
          auto connection = flowScene.createConnection();

          connect(connection, endToCheck, hit);

          auto address = std::make_pair(_id, hit);

          auto conPoint = connectionPointScenePosition(address, endToCheck);

          connection->setDraggingEnd(endToCheck);
          connection->connectToNode(address, conPoint);

          connection->setDraggingEnd(oppositeEnd(endToCheck));
        }
        else
        {
          auto connection = flowScene.getConnection(id);

          disconnect(connection, endToCheck, hit);

          connection->setDraggingEnd(endToCheck);
        }
      }
    };

  clickEnd(EndType::SINK);
  clickEnd(EndType::SOURCE);

  event->accept();
}


void
NodeGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  QPointF d = event->pos() - event->lastPos();

  if (event->lastPos() != event->pos())
    emit itemMoved(_id, d);

  QGraphicsObject::mouseMoveEvent(event);
}


void
NodeGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  _nodeGeometry.setHovered(true);
  update();
  event->accept();
}


void
NodeGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  _nodeGeometry.setHovered(false);
  update();
  event->accept();
}
