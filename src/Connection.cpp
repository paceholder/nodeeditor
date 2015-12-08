#include "Connection.hpp"
#include <QtWidgets/QtWidgets>

#include "FlowItem.hpp"
#include "FlowItemEntry.hpp"
#include "FlowScene.hpp"
#include "FlowGraphicsView.h"

#include <iostream>
#include <math.h>

Connection::
Connection(std::pair<QUuid, int> address,
           EndType dragging)
  : _id(QUuid::createUuid())
  , _source(10, 10)
  , _sink(100, 100)
  , _dragging(dragging)
  , _pointDiameter(10)
  , _animationPhase(0)
  , _lineWidth(3.0)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  FlowScene &flowScene = FlowScene::instance();
  flowScene.addItem(this);

  FlowItem* item = flowScene.getFlowItem(address.first);
  stackBefore(item);
  connect(item, &FlowItem::itemMoved, this, &Connection::onItemMoved);

  grabMouse();

  QPointF pointPos;
  switch (_dragging)
  {
    case  SOURCE:
    {
      _sinkAddress = address;
      pointPos     = mapFromScene(item->connectionPointPosition(address, SINK));

      //grabMouse();
      break;
    }

    case SINK:
    {
      _sourceAddress = address;
      pointPos       = mapFromScene(item->connectionPointPosition(address, SOURCE));

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
    case SOURCE:
      _sourceAddress = std::make_pair(QUuid(), -1);
      break;

    case SINK:
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
connectToFlowItem(std::pair<QUuid, int> address)
{
  Q_ASSERT(_dragging != NONE);

  switch (_dragging)
  {
    case SOURCE:
      _sourceAddress = address;
      break;

    case SINK:
      _sinkAddress = address;
      break;

    default:
      break;
  }

  _dragging = NONE;

  ungrabMouse();
}


QPointF
Connection::
endPointSceneCoordinate(EndType endType)
{
  QPointF result;

  switch (endType)
  {
    case SOURCE:
      result = _source;

      break;

    case SINK:
      result = _sink;
      break;

    default:
      break;
  }

  return result;
}


void
Connection::
paint(QPainter* painter,
      QStyleOptionGraphicsItem const*,
      QWidget*)
{
  if (!_sourceAddress.first.isNull())
  {
    FlowItem* item = FlowScene::instance().getFlowItem(_sourceAddress.first);
    _source = mapFromScene(item->connectionPointPosition(_sourceAddress.second,
                                                         SOURCE));
  }

  if (!_sinkAddress.first.isNull())
  {
    FlowItem* item = FlowScene::instance().getFlowItem(_sinkAddress.first);
    _sink = mapFromScene(item->connectionPointPosition(_sinkAddress.second,
                                                       SINK));
  }

  // --- bounding rect
  // painter->setPen(Qt::darkGray);
  // painter->drawRect(this->boundingRect());

  // ---- connection line
  QPen p;

  p.setWidth(_lineWidth);
  p.setColor(Qt::yellow);
  p.setColor(QColor(Qt::cyan).darker());
  painter->setPen(p);

  QPainterPath path(_source);

  double ratio1 = 0.3;
  double ratio2 = 1 - ratio1;

  QPointF c1(_sink.x() * ratio2 + _source.x() * ratio1, _source.y());

  QPointF c2(_sink.x() * ratio1 + _source.x() * ratio2, _sink.y());

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
  //

  return;
}


void
Connection::
mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  auto   distance  = [] (QPointF & d) { return sqrt (QPointF::dotProduct(d, d)); };
  double tolerance = 2.0 * _pointDiameter;

  {
    QPointF diff = event->pos() - _source;

    if (distance(diff) < tolerance)
    {
      _dragging = SOURCE;
      return;
    }
  }

  {
    QPointF diff = event->pos() - _sink;

    if (distance(diff) < tolerance)
    {
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
  // motion

  QPointF p = event->pos() - event->lastPos();

  prepareGeometryChange();

  switch (_dragging)
  {
    case SOURCE:
    {
      _source += p;
      break;
    }

    case SINK:
    {
      _sink += p;
      break;
    }

    default:
      break;
  }

  event->accept();
}


FlowItem*
Connection::
locateFlowItemAt(QPointF const &scenePoint,
                 QTransform const &transform)
{

  auto& scene = FlowScene::instance();

  QList<QGraphicsItem*> items = scene.items(scenePoint,
                                            Qt::IntersectsItemShape,
                                            Qt::DescendingOrder,
                                            transform);

  std::vector<QGraphicsItem*> filteredItems;

  std::copy_if (items.begin(),
                items.end(),
                std::back_inserter(filteredItems),
                [](QGraphicsItem * item)
                { return dynamic_cast<FlowItem*>(item); });

  if (filteredItems.empty())
    return nullptr;

  FlowItem* flowItem = dynamic_cast<FlowItem*>(filteredItems.front());

  return flowItem;
}


void
Connection::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  (void)event;


  // TODO why need this?
  FlowScene::instance().clearDraggingConnection();

  QPointF scenePoint     = mapToScene(event->pos());
  FlowGraphicsView* view = static_cast<FlowGraphicsView*>(event->widget());

  auto flowItem = locateFlowItemAt(scenePoint, view->transform());

  if (flowItem)
  {
    flowItem->tryConnect(this);
  }




  _dragging = NONE;

  ungrabMouse();

  event->accept();
}


void
Connection::
onItemMoved()
{
  prepareGeometryChange();

  if (!_sourceAddress.first.isNull())
  {
    FlowItem* item = FlowScene::instance().getFlowItem(_sourceAddress.first);
    _source = mapFromScene(item->connectionPointPosition(_sourceAddress.second,
                                                         SOURCE));
  }

  if (!_sinkAddress.first.isNull())
  {
    FlowItem* item = FlowScene::instance().getFlowItem(_sinkAddress.first);
    _sink = mapFromScene(item->connectionPointPosition(_sinkAddress.second,
                                                       SINK));
  }
}
