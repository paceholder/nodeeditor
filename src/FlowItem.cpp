#include "FlowItem.hpp"

#include <QtWidgets/QtWidgets>

#include <iostream>

#include "FlowItemEntry.hpp"
#include "FlowScene.hpp"
FlowItem::
FlowItem():
  _id(QUuid::createUuid()),
  _width(100),
  _height(150),
  _spacing(10),
  _connectionPointDiameter(12)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
}

void
FlowItem::
initializeFlowItem()
{
  initializeEntries();
  // embedQWidget();
  recalculateSize();
}

void
FlowItem::
initializeEntries()
{
  FlowItemEntry* entry = new FlowItemEntry(FlowItemEntry::SINK, _id);

  std::cout << "id " << _id.toString().toLocal8Bit().data() << std::endl;

  int height = entry->height();
  _sinkEntries.append(entry);

  std::cout << "create flow item entry " << std::endl;
  entry = new FlowItemEntry(FlowItemEntry::SINK, _id);

  _sinkEntries.append(entry);

  entry = new FlowItemEntry(FlowItemEntry::SINK, _id);

  _sinkEntries.append(entry);

  int totalHeight = 0;

  for (int i = 0; i < _sinkEntries.size(); ++i) {
    _sinkEntries[i]->setPos(0, totalHeight + _spacing / 2);
    totalHeight += _sinkEntries[i]->height() + _spacing;
  }

  ////////////////////////////

  entry = new FlowItemEntry(FlowItemEntry::SOURCE, _id);

  _sourceEntries.append(entry);

  entry = new FlowItemEntry(FlowItemEntry::SOURCE, _id);

  _sourceEntries.append(entry);

  totalHeight += _spacing;

  for (int i = 0; i < _sourceEntries.size(); ++i) {
    _sourceEntries[i]->setPos(0, totalHeight + _spacing / 2);
    totalHeight += _sourceEntries[i]->height() + _spacing;
  }
}

void
FlowItem::
embedQWidget()
{
  QPushButton* button = new QPushButton(QString("Hello"));

  QGraphicsProxyWidget* proxyWidget = new QGraphicsProxyWidget();

  proxyWidget->setWidget(button);

  button->setVisible(true);
  proxyWidget->setParentItem(this);
}

void
FlowItem::
recalculateSize()
{
  int totalHeight = 0;

  for (int i = 0; i < _sinkEntries.size(); ++i)
    totalHeight += _sinkEntries[i]->height() + _spacing;

  totalHeight += _spacing;

  for (int i = 0; i < _sourceEntries.size(); ++i)
    totalHeight += _sourceEntries[i]->height() + _spacing;

  _height = totalHeight;
}

QRectF
FlowItem::
boundingRect() const
{
  double addon = 3 * _connectionPointDiameter;
  return QRectF(0 - addon, 0 - addon,
                _width + 2 * addon, _height + 2 * addon);
}

QUuid
FlowItem::
id() { return _id; }

QPointF
FlowItem::
sinkPointPos(int index) const
{
  double totalHeight = 0;

  for (int i = 1; i <= index; ++i)
    totalHeight += _sinkEntries[i]->height() + _spacing;

  totalHeight += _spacing / 2 + _sinkEntries[index]->height() / 2;
  double x = 0.0 - _connectionPointDiameter * 1.3;

  return mapToScene(QPointF(x, totalHeight));
}

QPointF
FlowItem::
sourcePointPos(int index) const
{
  double totalHeight = 0;

  for (int i = 0; i < _sinkEntries.size(); ++i)
    totalHeight += _sinkEntries[i]->height() + _spacing;

  totalHeight += _spacing;

  for (int i = 1; i <= index; ++i)
    totalHeight += _sourceEntries[i]->height() + _spacing;

  totalHeight += (_spacing  + _sourceEntries[index]->height()) / 2.0;
  double x = _width + _connectionPointDiameter * 1.3;

  return mapToScene(QPointF(x, totalHeight));
}

void
FlowItem::
paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  painter->setPen(Qt::white);
  painter->setBrush(QColor(Qt::darkGray));
  QRectF boundary(0 - _connectionPointDiameter,
                  0 - _connectionPointDiameter,
                  _width + 2 *  _connectionPointDiameter,
                  _height + 2 * _connectionPointDiameter);

  painter->drawRoundedRect(boundary, 10.0, 10.0);

  drawConnectionPoints(painter);

  drawFilledConnectionPoints(painter);
}

void
FlowItem::
drawConnectionPoints(QPainter* painter)
{
  painter->setBrush(QColor(Qt::gray).darker());
  double totalHeight = 0;

  for (int i = 0; i < _sinkEntries.size(); ++i) {
    double h = _sinkEntries[i]->height();

    double y = totalHeight + (_spacing  + h) / 2;
    double x = 0.0 - _connectionPointDiameter * 1.3;
    painter->drawEllipse(QPointF(x, y),
                         _connectionPointDiameter * 0.6,
                         _connectionPointDiameter * 0.6);

    totalHeight += h + _spacing;
  }

  totalHeight += _spacing;

  for (int i = 0; i < _sourceEntries.size(); ++i) {
    double h = _sourceEntries[i]->height();

    double y = totalHeight + (_spacing  + h) / 2;
    double x = _width + _connectionPointDiameter * 1.3;
    painter->drawEllipse(QPointF(x, y),
                         _connectionPointDiameter * 0.6,
                         _connectionPointDiameter * 0.6);

    totalHeight += h + _spacing;
  }
}

void
FlowItem::
drawFilledConnectionPoints(QPainter* painter)
{
  painter->setPen(Qt::cyan);
  painter->setBrush(Qt::cyan);

  double totalHeight = 0;

  for (int i = 0; i < _sinkEntries.size(); ++i) {
    double h = _sinkEntries[i]->height();

    double y = totalHeight + (_spacing  + h) / 2;
    double x = 0.0 - _connectionPointDiameter * 1.3;

    if (!_sinkEntries[i]->getConnectionID().isNull())
      painter->drawEllipse(QPointF(x, y),
                           _connectionPointDiameter * 0.4,
                           _connectionPointDiameter * 0.4);

    totalHeight += h + _spacing;
  }

  totalHeight += _spacing;

  for (int i = 0; i < _sourceEntries.size(); ++i) {
    double h = _sourceEntries[i]->height();

    double y = totalHeight + (_spacing  + h) / 2;
    double x = _width + _connectionPointDiameter * 1.3;

    if (!_sourceEntries[i]->getConnectionID().isNull())
      painter->drawEllipse(QPointF(x, y),
                           _connectionPointDiameter * 0.4,
                           _connectionPointDiameter * 0.4);

    totalHeight += h + _spacing;
  }
}

int
FlowItem::
checkHitSinkPoint(const QPointF eventPoint) const
{
  int result = -1;

  // labmda function for Euclidian distance
  auto distance = [](QPointF& d) { return sqrt(d.x() * d.x() +
                                               d.y() * d.y()); };

  double tolerance = 1.0 * _connectionPointDiameter;

  for (int i = 0; i < _sinkEntries.size(); ++i) {
    QPointF p = sinkPointPos(i) - eventPoint;

    if (distance(p) < tolerance)
      result = i;
  }

  return result;
}

int
FlowItem::
checkHitSourcePoint(const QPointF eventPoint) const
{
  int result = -1;

  // labmda function for Euclidian distance
  auto distance = [](QPointF& d) { return sqrt(d.x() * d.x() +
                                               d.y() * d.y()); };

  double tolerance = 1.0 * _connectionPointDiameter;

  for (int i = 0; i < _sourceEntries.size(); ++i) {
    QPointF p = sourcePointPos(i) - eventPoint;

    if (distance(p) < tolerance)
      result = i;
  }

  return result;
}

void
FlowItem::
mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  int hit = checkHitSinkPoint(mapToScene(event->pos()));

  if (hit >= 0 && _sinkEntries[hit]->getConnectionID().isNull()) {
    QUuid connectionID =
      FlowScene::instance()->createConnection(_id, hit, Connection::SOURCE);

    FlowItemEntry* entry = _sinkEntries[hit];
    entry->setConnectionID(connectionID);
  }

  hit = checkHitSourcePoint(mapToScene(event->pos()));

  if (hit >= 0 && _sourceEntries[hit]->getConnectionID().isNull()) {
    QUuid connectionID =
      FlowScene::instance()->createConnection(_id, hit, Connection::SINK);

    FlowItemEntry* entry = _sourceEntries[hit];
    entry->setConnectionID(connectionID);
  }

  // event->ignore();
  // QGraphicsObject::mousePressEvent(event);
}

void
FlowItem::
mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (event->lastPos() != event->pos())
    emit itemMoved();

  QGraphicsObject::mouseMoveEvent(event);
}
