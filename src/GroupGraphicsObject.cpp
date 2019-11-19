#include "GroupGraphicsObject.hpp"

#include "FlowScene.hpp"

using QtNodes::GroupRectItem;
using QtNodes::NodeGroup;
using QtNodes::GroupGraphicsObject;

GroupRectItem::
GroupRectItem(QRectF rect,
              QGraphicsObject* parent)
  : QGraphicsRectItem(rect, parent)
{}

void
GroupRectItem::
paint(QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget)
{
  painter->setBrush(QColor("#10a5b084"));
  painter->setPen(Qt::PenStyle::DashLine);
  painter->drawRoundedRect(rect(), roundedBorderRadius, roundedBorderRadius);
}

QRectF
GroupRectItem::
boundingRect() const
{
  return rect();
}

GroupGraphicsObject::
GroupGraphicsObject(QtNodes::FlowScene& scene, QtNodes::NodeGroup& nodeGroup)
  : _scene(scene)
  , _group(nodeGroup)
  , _areaRect(QRectF(0, 0, 50, 50), this)
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);

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
}
