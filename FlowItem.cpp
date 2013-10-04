#include "FlowItem.hpp"

#include <QtWidgets/QtWidgets>

#include <iostream>

#include "FlowItemEntry.hpp"
FlowItem::
FlowItem():
  _width(100),
  _height(150),
  _connectionPointDiameter(12)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  initializeEntries();
  recalculateSize();
}

void
FlowItem::
initializeEntries()
{
  FlowItemEntry* entry = new FlowItemEntry(FlowItemEntry::SINK, this);

  int height = entry->height();
  _sinkEntries.append(entry);

  entry = new FlowItemEntry(FlowItemEntry::SINK, this);

  _sinkEntries.append(entry);

  entry = new FlowItemEntry(FlowItemEntry::SINK, this);

  _sinkEntries.append(entry);

  for (int i = 0; i < _sinkEntries.size(); ++i)
    _sinkEntries[i]->setPos(0, i * (_sinkEntries.size() + height + 6) + 2);
}

void
FlowItem::
recalculateSize()
{
  int height = 0;

  if (_sinkEntries.size() > 0)
    height = _sinkEntries[0]->height();

  _height = _sinkEntries.size() * (height + 6);
}

QRectF
FlowItem::
boundingRect() const
{
  double addon = _connectionPointDiameter;
  return QRectF(0 - addon, 0 - addon,
                _width + 2 * addon, _height + 2 * addon);
}

void
FlowItem::
paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  painter->setPen(Qt::white);
  painter->setBrush(QColor(Qt::gray).darker());
  painter->drawRoundedRect(this->boundingRect(), 10.0, 10.0);
}
