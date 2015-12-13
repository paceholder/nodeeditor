#include "Node.hpp"

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include <iostream>
#include <cstdlib>

#include "ConnectionGraphicsObject.hpp"
#include "FlowItemEntry.hpp"
#include "FlowScene.hpp"
#include "NodePainter.hpp"

Node::
Node()
  : _id(QUuid::createUuid())
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  //auto effect = new QGraphicsDropShadowEffect;
  //effect->setOffset(4, 4);
  //effect->setBlurRadius(20);
  //effect->setColor(QColor(Qt::gray).darker(800));

  //setGraphicsEffect(effect);
}


void
Node::
initializeNode()
{
  setAcceptHoverEvents(true);

  initializeEntries();

  //embedQWidget();

  _nodeGeometry.recalculateSize();
}


void
Node::
embedQWidget()
{
  QPushButton* button = new QPushButton(QString("Hello"));

  QGraphicsProxyWidget* proxyWidget = new QGraphicsProxyWidget();

  proxyWidget->setWidget(button);

  button->setVisible(true);
  proxyWidget->setParentItem(this);
}


QRectF
Node::
boundingRect() const
{
  return _nodeGeometry.boundingRect();
}


QPointF
Node::
connectionPointScenePosition(std::pair<QUuid, int> address,
                             EndType endType) const
{
  return mapToScene(_nodeGeometry.connectionPointScenePosition(address.second,
                                                               endType));
}


QPointF
Node::
connectionPointScenePosition(int index,
                             EndType endType) const
{
  return mapToScene(_nodeGeometry.connectionPointScenePosition(index,
                                                               endType));
}


void
Node::
reactToPossibleConnection(EndType,
                          QPointF const &scenePoint)
{
  _nodeGeometry.setDraggingPosition(mapFromScene(scenePoint));
}


bool
Node::
canConnect(EndType draggingEnd, QPointF const &scenePoint)
{
  auto &entries = getEntryArray(draggingEnd);
  int  hit      = checkHitScenePoint(draggingEnd, scenePoint);

  return (hit >= 0 &&
          entries[hit]->getConnectionID().isNull());
}


std::pair<QUuid, int>
Node::
connect(Connection const* connection,
        EndType draggingEnd,
        int hit)
{
  auto &entries = getEntryArray(draggingEnd);

  entries[hit]->setConnectionID(connection->id());

  QObject::connect(this, &Node::itemMoved,
                   connection->getConnectionGraphicsObject(),
                   &ConnectionGraphicsObject::onItemMoved);

  connection->getConnectionGraphicsObject()->stackBefore(this);

  auto address = std::make_pair(_id, hit);

  return address;
}


std::pair<QUuid, int>
Node::
connect(Connection const* connection,
        EndType draggingEnd,
        QPointF const& scenePoint)
{
  int hit = checkHitScenePoint(draggingEnd, scenePoint);

  return connect(connection, draggingEnd, hit);
}


void
Node::
disconnect(Connection const* connection,
           EndType endType,
           int hit)
{
  QObject::disconnect(this, &Node::itemMoved,
                      connection->getConnectionGraphicsObject(),
                      &ConnectionGraphicsObject::onItemMoved);

  auto& entries = getEntryArray(endType);

  entries[hit]->setConnectionID(QUuid());
}


void
Node::
paint(QPainter* painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* )
{
  painter->setClipRect(option->exposedRect);

  NodePainter::paint(painter, _nodeGeometry,
                     getEntryArray(EndType::SOURCE),
                     getEntryArray(EndType::SINK));
}


// todo make unsigned, define invalid #
int
Node::
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
Node::
checkHitSinkScenePoint(const QPointF eventPoint) const
{
  int result = -1;

  auto   diameter  = _nodeGeometry.connectionPointDiameter();
  double tolerance = 1.0 * diameter;

  for (size_t i = 0; i < _sinkEntries.size(); ++i)
  {
    QPointF p = connectionPointScenePosition(i, EndType::SINK) - eventPoint;

    auto distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
      result = i;
  }

  return result;
}


int
Node::
checkHitSourceScenePoint(const QPointF eventPoint) const
{
  int result = -1;

  auto   diameter  = _nodeGeometry.connectionPointDiameter();
  double tolerance = 1.0 * diameter;

  for (size_t i = 0; i < _sourceEntries.size(); ++i)
  {
    QPointF p = connectionPointScenePosition(i, EndType::SOURCE) - eventPoint;
    auto    distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
      result = i;
  }

  return result;
}


void
Node::
mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  auto clickEnd =
    [&](EndType endToCheck)
    {
      int hit = checkHitScenePoint(endToCheck,
                                   event->scenePos());

      FlowScene &flowScene = FlowScene::instance();

      if (hit >= 0)
      {
        auto& entries = getEntryArray(endToCheck);

        // node's sink has no connection

        QUuid const id = entries[hit]->getConnectionID();

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

  //event->ignore();
}


void
Node::
mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QPointF d = event->pos() - event->lastPos();

  if (event->lastPos() != event->pos())
    emit itemMoved(_id, d);

  QGraphicsObject::mouseMoveEvent(event);
}


void
Node::
hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  _nodeGeometry.setHovered(true);
  update();
  event->accept();
}


void
Node::
hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  _nodeGeometry.setHovered(false);
  update();
  event->accept();
}


void
Node::
initializeEntries()
{
  unsigned int n = std::rand() % 4 + 2;

  for (auto i = 0ul; i < n; ++i)
  {
    FlowItemEntry* entry =
      new FlowItemEntry(EndType::SINK, _id, _nodeGeometry);

    _sinkEntries.push_back(entry);
  }

  int totalHeight = 0;
  for (size_t i = 0; i < _sinkEntries.size(); ++i)
  {
    _sinkEntries[i]->setPos(0, totalHeight + _nodeGeometry.spacing() / 2);
    totalHeight += _nodeGeometry.entryHeight() + _nodeGeometry.spacing();
  }

  _nodeGeometry.setNSinks(n);

  ////////////////////////////

  n = std::rand() % 4 + 2;

  for (auto i = 0ul; i < n; ++i)
  {
    FlowItemEntry* entry =
      new FlowItemEntry(EndType::SOURCE, _id, _nodeGeometry);

    _sourceEntries.push_back(entry);
  }

  totalHeight += _nodeGeometry.spacing();

  for (size_t i = 0; i < _sourceEntries.size(); ++i)
  {
    _sourceEntries[i]->setPos(0, totalHeight + _nodeGeometry.spacing() / 2);
    totalHeight += _nodeGeometry.entryHeight() + _nodeGeometry.spacing();
  }

  _nodeGeometry.setNSources(n);
}


std::vector<FlowItemEntry*>&
Node::
getEntryArray(EndType endType)
{
  switch (endType)
  {
    case EndType::SOURCE:
      return _sourceEntries;
      break;

    case EndType::SINK:
      return _sinkEntries;
      break;

    default:
      break;
  }
}
