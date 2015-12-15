#include "FlowItemEntry.hpp"
#include <QtGui/QPainter>

#include "Node.hpp"
#include "FlowScene.hpp"

FlowItemEntry::
FlowItemEntry(EndType type,
              QUuid parentID,
              NodeGeometry const& nodeGeom,
              QString name,
              QUuid connectionID)
  : _id(QUuid::createUuid())
  , _parentID(parentID)
  , _connectionID(connectionID)
  , _type(type)
  , _name(name)
  , _nodeGeometry(nodeGeom)
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
  return _nodeGeometry.entryBoundingRect();
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
      QStyleOptionGraphicsItem const*,
      QWidget*)
{

  return;

  painter->setPen(Qt::white);
  painter->setBrush(QColor(Qt::darkGray));
  painter->drawRect(boundingRect());
}
