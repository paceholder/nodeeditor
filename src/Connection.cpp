#include "Connection.hpp"
#include <QtWidgets/QtWidgets>

#include "Node.hpp"
#include "FlowItemEntry.hpp"
#include "FlowScene.hpp"
#include "FlowGraphicsView.h"

#include <iostream>
#include <math.h>

#define DEBUG_DRAWING 1

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

  // TODO why need this?
  //FlowScene::instance().clearDraggingConnection();

  auto node = FlowScene::locateNodeAt(event);

  if (node)

    _connection.tryConnectToNode(node, event->scenePos());

  {
    flowItem->tryConnect(this);
  }

  //----------------

  {

    // TODO call from connection
    _connection.setDraggingEnd(EndType::NONE);

    ungrabMouse();

    event->accept();
  }
}


Connection::
Connection(std::pair<QUuid, int> address,
           EndType dragging)
  : _id(QUuid::createUuid())
  , _dragging(dragging)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  FlowScene &flowScene = FlowScene::instance();
  flowScene.addItem(this);

  Node* item = flowScene.getNode(address.first);
  stackBefore(item);
  connect(item, &Node::itemMoved, this, &Connection::onItemMoved);

  grabMouse();

  QPointF pointPos;
  switch (_dragging)
  {
    case  EndType::SOURCE:
    {
      _sinkAddress = address;
      pointPos     = mapFromScene(item->connectionPointScenePosition(address, EndType::SINK));

      //grabMouse();
      break;
    }

    case EndType::SINK:
    {
      _sourceAddress = address;
      pointPos       = mapFromScene(item->connectionPointScenePosition(address, EndType::SOURCE));

      //grabMouse();
      break;
    }

    default:
      // should not get to here
      break;
  }

  _source = pointPos;
  _sink   = pointPos;

  auto effect = new QGraphicsDropShadowEffect;
  effect->setOffset(4, 4);
  effect->setBlurRadius(20);
  effect->setColor(QColor(Qt::gray).darker(800));

  setGraphicsEffect(effect);
}


QUuid
Connection::
id()
{
  return _id;
}


void
Connection::
setDragging(EndType dragging)
{
  _dragging = dragging;

  grabMouse();

  switch (_dragging)
  {
    case EndType::SOURCE:
      _sourceAddress = std::make_pair(QUuid(), -1);
      break;

    case EndType::SINK:
      _sinkAddress = std::make_pair(QUuid(), -1);
      break;

    default:
      break;
  }
}


QRectF
Connection::
boundingRect() const
{
  double  protectOvershooting = 20;
  QPointF addon(_pointDiameter + protectOvershooting,
                _pointDiameter + protectOvershooting);

  QPointF minimum(qMin(_source.x(), _sink.x()),
                  qMin(_source.y(), _sink.y()));

  QPointF maximum(qMax(_source.x(), _sink.x()),
                  qMax(_source.y(), _sink.y()));

  return QRectF(minimum - addon, maximum + addon);
}


void
Connection::
advance(int )
{
  // if (phase == 1) _animationPhase = (_animationPhase + 1) % 7;
}


void
Connection::
timerEvent(QTimerEvent*)
{
  // this->advance(1);
}


void
Connection::
tryConnectToNode(Node* node, QPointF scenePoint)
{
  bool ok = node->canConnect(_draggingEnd, scenePoint);

  if (ok)
  {
    auto address = node->connect(this, _draggingEnd, scenePoint);


    _draggingEnd = EndType::NONE;

    // ungrabMouse

  }
}


void
Connection::
connectToNode(std::pair<QUuid, int> address)
{
  Q_ASSERT(_dragging != EndType::NONE);

  Node* item = FlowScene::instance().getNode(address.first);

  switch (_dragging)
  {
    case EndType::SOURCE:
      _sourceAddress = address;
      _source = mapFromScene(item->connectionPointScenePosition(_sourceAddress.second,
                                                                EndType::SOURCE));
      break;

    case EndType::SINK:
      _sinkAddress = address;
      _sink = mapFromScene(item->connectionPointScenePosition(_sinkAddress.second,
                                                              EndType::SINK));
      break;

    default:
      break;
  }

  _dragging = EndType::NONE;

  ungrabMouse();

  update();
}


QPointF
Connection::
endPointSceneCoordinate(EndType endType)
{
  QPointF result;

  switch (endType)
  {
    case EndType::SOURCE:
      result = _source;

      break;

    case EndType::SINK:
      result = _sink;
      break;

    default:
      break;
  }

  return mapToScene(result);
}


void
Connection::
paint(QPainter* painter,
      QStyleOptionGraphicsItem const*,
      QWidget*)
{

  double const ratio1 = 0.5;
  double const ratio2 = 1 - ratio1;
  QPointF c1(_sink.x() * ratio2 + _source.x() * ratio1, _source.y());
  QPointF c2(_sink.x() * ratio1 + _source.x() * ratio2, _sink.y());

#ifdef DEBUG_DRAWING

  {
    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);

    painter->drawLine(QLineF(_source, c1));
    painter->drawLine(QLineF(c1, c2));
    painter->drawLine(QLineF(c2, _sink));
    painter->drawEllipse(c1, 4, 4);
    painter->drawEllipse(c2, 4, 4);
  }
#endif

  QPen p;
  p.setWidth(_lineWidth);
  p.setColor(QColor(Qt::cyan).darker());
  painter->setPen(p);
  painter->setBrush(Qt::NoBrush);

  // cubic spline
  QPainterPath path(_source);
  path.cubicTo(c1, c2, _sink);
  painter->drawPath(path);

  //path.angleAtPercent(t)

  painter->setPen(Qt::white);
  painter->setBrush(Qt::white);
  painter->drawEllipse(_source, _pointDiameter / 2, _pointDiameter / 2);
  painter->drawEllipse(_sink, _pointDiameter / 2, _pointDiameter / 2);
}


void
Connection::
onItemMoved()
{
  prepareGeometryChange();

  if (!_sourceAddress.first.isNull())
  {
    Node* item = FlowScene::instance().getNode(_sourceAddress.first);
    _source = mapFromScene(item->connectionPointScenePosition(_sourceAddress.second,
                                                              EndType::SOURCE));
  }

  if (!_sinkAddress.first.isNull())
  {
    Node* item = FlowScene::instance().getNode(_sinkAddress.first);
    _sink = mapFromScene(item->connectionPointScenePosition(_sinkAddress.second,
                                                            EndType::SINK));
  }
}
