#include "Connection.hpp"
#include <QtWidgets/QtWidgets>

#include "FlowItem.hpp"
#include "FlowScene.hpp"

#include <iostream>
#include <math.h>
Connection::
Connection(QUuid flowItemSourceID,
           QUuid flowItemSinkID):
  _id(QUuid::createUuid()),
  _flowItemSourceID(flowItemSourceID),
  _flowItemSinkID(flowItemSinkID),
  _source(10, 10),
  _sink(100, 100),
  _dragging(NONE),
  _pointDiameter(12),
  _animationPhase(0)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  startTimer(200);
}

void
Connection::
initializeConnection()
{
  if (_flowItemSourceID.isNull() && !_flowItemSinkID.isNull()) {
    // first, we set correct coordinate for a fixed point

    // we drag source point
    _dragging = SOURCE;

    FlowItem* item = FlowScene::instance()->getFlowItem(_flowItemSinkID);

    QPointF pointPos = mapFromScene(item->sinkPointPos(0));

    _source = pointPos;
    _sink   = pointPos;

    grabMouse();
  } else if (!_flowItemSourceID.isNull() && _flowItemSinkID.isNull()) {
    // we drag sink point
    _dragging = SINK;
    grabMouse();
  }
}

QRectF
Connection::
boundingRect() const
{
  QPointF addon(_pointDiameter, _pointDiameter);

  QPointF minimum(qMin(_source.x(), _sink.x()),
                  qMin(_source.y(), _sink.y()));

  QPointF maximum(qMax(_source.x(), _sink.x()),
                  qMax(_source.y(), _sink.y()));

  return QRectF(minimum - addon, maximum + addon);
}

void
Connection::
advance(int phase)
{
  if (phase == 1)
    _animationPhase = (_animationPhase + 1) % 7;
}

void
Connection::
timerEvent(QTimerEvent* event)
{
  this->advance(1);
}

void
Connection::
paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  // --- bounding rect
  // painter->setPen(Qt::darkGray);
  // painter->drawRect(this->boundingRect());

  // ---- connection line
  QPen p;
  p.setWidth(2);
  p.setColor(Qt::yellow);
  p.setColor(Qt::cyan);
  painter->setPen(p);

  QPainterPath path(_source);

  QPointF c1((_sink.x() + _source.x()) / 2.0, _source.y());

  QPointF c2((_sink.x() + _source.x()) / 2.0, _sink.y());

  path.cubicTo(c1, c2, _sink);

  painter->drawPath(path);

  // ----- draw bspline knots

  // painter->setPen(Qt::white);
  // painter->setBrush(Qt::red);
  // painter->drawEllipse(c1, 4, 4);
  // painter->setBrush(Qt::blue);
  // painter->drawEllipse(c2, 4, 4);

  // ---- draw _sink and _source

  painter->setPen(Qt::white);
  painter->setBrush(Qt::white);
  painter->drawEllipse(_source, _pointDiameter / 2, _pointDiameter / 2);
  painter->drawEllipse(_sink, _pointDiameter / 2, _pointDiameter / 2);

  p.setWidth(1);
  p.setColor(Qt::yellow);
  painter->setBrush(Qt::yellow);
  painter->setPen(p);

  // for (int i = 1; i < 70; ++i) {
  // double t = i / (double)70;

  // if ((i + _animationPhase) % 7 == 0) {
  // QPoint p;
  // p.setX(B(_source.x(), c1.x(), c2.x(), _sink.x(), t));

  // p.setY(B(_source.y(), c1.y(), c2.y(), _sink.y(), t));

  // painter->drawEllipse(p, 2, 2);
  // }
  // }

  return;
}

void
Connection::
mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  auto   distance  = [](QPointF& d) { return sqrt(d.x() * d.x() + d.y() * d.y()); };
  double tolerance = 2.0 * _pointDiameter;

  {
    QPointF diff = event->pos() - _source;

    if (distance(diff) < tolerance) {
      _dragging = SOURCE;
      return;
    }
  }

  {
    QPointF diff = event->pos() - _sink;

    if (distance(diff) < tolerance) {
      _dragging = SINK;
      return;
    }
  }

  event->ignore();
}

void
Connection::
mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QPointF p = event->pos() - event->lastPos();

  prepareGeometryChange();

  switch (_dragging) {
    case SOURCE: {
      _source += p;
      break;
    }

    case SINK: {
      _sink += p;
      break;
    }

    default:
      break;
  }

  // std::cout << _source.x() << "   " << _source.y() << std::endl;
}

void
Connection::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  _dragging = NONE;
  ungrabMouse();
}
