#include "Connection.hpp"
#include <QtWidgets/QtWidgets>

#include <iostream>
#include <math.h>

static const double Pi = 3.1415926535897932384626433819717;

void
ConnectionPoint::
paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  painter->setBrush(Qt::black);
  painter->drawEllipse(QPointF(0, 0), 4, 4);

  this->setFlag(QGraphicsItem::ItemIsMovable, true);
  this->setFlag(QGraphicsItem::ItemIsFocusable, true);
}

void
ConnectionPoint::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  parentItem()->update();
}

// void
// ConnectionPoint::
// mouseMoveEvent(QGraphicsSceneMouseEvent* event)
// {
// parentItem()->update();
// }

QRectF
ConnectionPoint::
boundingRect() const
{
  return QRectF(-2, -2, 4, 4);
}
// --------------------------

Connection::
Connection(int BlockOutID, int BlockInID):
  _animationPhase(0)
{
  this->setFlag(QGraphicsItem::ItemIsMovable, false);

  source = new ConnectionPoint(this);

  source->setPos(30, 20);

  sink = new ConnectionPoint(this);

  sink->setPos(100, 100);

  // setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  startTimer(200);
}

QRectF
Connection::
boundingRect() const
{
  return QRectF(source->pos(), source->pos() + sink->pos());
}

void
Connection::
advance(int phase)
{
  if (phase == 1)
    _animationPhase = (_animationPhase + 1) % 7;

  //    std::cout << "phase 1" << std::endl;
}

void
Connection::
timerEvent(QTimerEvent* event)
{
  this->advance(1);
  // this->update();
}
double
B(double one, double two, double three, double four, double t)
{
  return (1 - t) * (1 - t) * (1 - t) * one
         + 3 * (1 - t) * (1 - t) * t * two
         + 3 * (1 - t) * t * t * three
         + t * t * t * four;
}

void
Connection::
paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  painter->setPen(Qt::white);
  painter->drawRect(this->boundingRect());

  QPen p;
  p.setWidth(2);
  p.setColor(Qt::yellow);
  painter->setPen(p);

  QPainterPath path(source->pos());

  QPointF c1((sink->pos().x() + source->pos().x()) / 2.0, source->pos().y());

  QPointF c2((sink->pos().x() + source->pos().x()) / 2.0, sink->pos().y());

  path.cubicTo(c1, c2, sink->pos());

  painter->drawPath(path);

  painter->setPen(Qt::white);
  painter->setBrush(Qt::red);
  painter->drawEllipse(c1, 4, 4);
  painter->setBrush(Qt::blue);
  painter->drawEllipse(c2, 4, 4);

  p.setWidth(1);
  p.setColor(Qt::yellow);
  painter->setBrush(Qt::yellow);
  painter->setPen(p);

  for (int i = 1; i < 70; ++i) {
    double t = i / (double)70;

    if ((i + _animationPhase) % 7 == 0) {
      QPoint p;
      p.setX(B(source->pos().x(), c1.x(), c2.x(), sink->pos().x(), t));

      p.setY(B(source->pos().y(), c1.y(), c2.y(), sink->pos().y(), t));

      painter->drawEllipse(p, 2, 2);
    }
  }

  return;
}

void
Connection::
mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  prepareGeometryChange();
}
