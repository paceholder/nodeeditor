#include "FlowItemEntry.hpp"
#include <QtGui/QPainter>

#include "FlowItem.hpp"
#include "FlowScene.hpp"
FlowItemEntry::
FlowItemEntry(Type type, QUuid parentID, QUuid connectionID):
  _type(type),
  _parentID(parentID),
  _connectionID(connectionID),
  _width(100),
  _height(20)
{
  FlowItem* flowItem = FlowScene::instance()->getFlowItem(_parentID);
  this->setParentItem(flowItem);
}

QUuid
FlowItemEntry::
id() const { return _id; }

QUuid
FlowItemEntry::
parentID() const
{
  return _parentID;
}

QRectF
FlowItemEntry::
boundingRect() const
{
  double addon = 0;
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
