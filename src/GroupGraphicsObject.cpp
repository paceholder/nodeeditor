#include "GroupGraphicsObject.hpp"

#include "FlowScene.hpp"

using QtNodes::NodeGroup;
using QtNodes::GroupGraphicsObject;

GroupGraphicsObject::
GroupGraphicsObject(QtNodes::FlowScene& scene, QtNodes::NodeGroup& nodeGroup)
{

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
}

void QtNodes::GroupGraphicsObject::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{

}
