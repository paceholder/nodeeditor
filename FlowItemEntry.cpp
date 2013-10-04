#include "FlowItemEntry.hpp"

#include <QtGui/QPainter>
FlowItemEntry::
FlowItemEntry(Type type, QGraphicsItem* parent):
  QGraphicsObject(parent),
  _type(type),
  _width(100),
  _height(20)
{
  //
}

QRectF
FlowItemEntry::
boundingRect() const
{
  double addon = 2;
  return QRectF(0 - addon, 0 - addon,
                _width + 2 * addon, _height + 2 * addon);
}

void
FlowItemEntry::
paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  painter->setPen(Qt::white);
  painter->setBrush(QColor(Qt::darkGray));
  painter->drawRoundedRect(this->boundingRect(), 3.0, 3.0);
}
