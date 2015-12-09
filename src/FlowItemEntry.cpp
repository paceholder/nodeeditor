#include "FlowItemEntry.hpp"
#include <QtGui/QPainter>

#include "Node.hpp"
#include "FlowScene.hpp"

FlowItemEntry::
FlowItemEntry(EndType type, QUuid parentID, QString name, QUuid connectionID)
  : _id(QUuid::createUuid())
  , _parentID(parentID)
  , _connectionID(connectionID)
  , _type(type)
  , _name(name)
  , _width(100)
  , _height(20)
{
  Node* flowItem = FlowScene::instance().getNode(_parentID);

  setParentItem(flowItem);
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

  return QRectF(0 - addon,
                0 - addon,
                _width + 2 * addon,
                _height + 2 * addon);
}


void
FlowItemEntry::
setConnectionID(QUuid connectionID)
{
  _connectionID = connectionID;
}


QUuid
FlowItemEntry::
getConnectionID() const
{
  return _connectionID;
}


void
FlowItemEntry::
paint(QPainter* painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* widget)
{
  Q_UNUSED(option);
  Q_UNUSED(widget);

  painter->setPen(Qt::white);
  painter->setBrush(QColor(Qt::darkGray));
  painter->drawRect(boundingRect());
}
