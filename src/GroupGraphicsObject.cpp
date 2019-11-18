#include "GroupGraphicsObject.hpp"

#include "FlowScene.hpp"

using QtNodes::NodeGroup;
using QtNodes::GroupGraphicsObject;

GroupGraphicsObject::
GroupGraphicsObject(QtNodes::FlowScene& scene, QtNodes::NodeGroup& nodeGroup)
  : _scene(scene)
  , _group(nodeGroup)
  , _areaRect(QRectF(0, 0, 50, 50))
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setOpacity(0.5);
}

GroupGraphicsObject::
~GroupGraphicsObject()
{
  _scene.removeItem(this);
}

NodeGroup&
GroupGraphicsObject::
group()
{
  return _group;
}

NodeGroup const&
GroupGraphicsObject::
group() const
{
  return _group;
}

QRectF
GroupGraphicsObject::
boundingRect() const
{
  return QRectF();
}

void
GroupGraphicsObject::
paint(QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget)
{
  painter->setBrush(Qt::lightGray);
  painter->setPen(Qt::PenStyle::DashLine);
  painter->drawRoundedRect(_areaRect, 8.0, 8.0);
}
