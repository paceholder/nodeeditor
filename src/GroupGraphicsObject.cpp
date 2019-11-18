#include "GroupGraphicsObject.hpp"

#include "FlowScene.hpp"

using QtNodes::NodeGroup;
using QtNodes::GroupGraphicsObject;

GroupGraphicsObject::
GroupGraphicsObject(QtNodes::FlowScene& scene, QtNodes::NodeGroup& nodeGroup)
  : _scene(scene)
  , _group(nodeGroup)
{
  _scene.addItem(this);

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
  painter->setBrush(Qt::darkBlue);
  painter->setPen(Qt::PenStyle::DashLine);
  painter->drawRect(QRectF(0, 0, 30, 30));
}
