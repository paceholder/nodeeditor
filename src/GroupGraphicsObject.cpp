#include "GroupGraphicsObject.hpp"

#include "FlowScene.hpp"

using QtNodes::GroupRectItem;
using QtNodes::NodeGroup;
using QtNodes::GroupGraphicsObject;

GroupRectItem::
GroupRectItem(QGraphicsObject* parent,
              QRectF rect)
  : QGraphicsRectItem(rect, parent)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  _currentColor = _fillColor;
}

void
GroupRectItem::
paint(QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget)
{
  painter->setBrush(_currentColor);
  painter->setPen(Qt::PenStyle::DashLine);
  painter->drawRoundedRect(rect(), roundedBorderRadius, roundedBorderRadius);
}

QRectF
GroupRectItem::
boundingRect() const
{
  return rect();
}

//QPointF
//GroupGraphicsObject::
//centerInSceneCoords() const
//{
//  return mapToScene(boundingRect().center());
//}

//QPointF
//GroupGraphicsObject::
//topLeftInSceneCoords() const
//{
//  return mapToScene(boundingRect().topLeft());
//}

void
GroupGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  _areaRect.setHoverColor();
  for(auto& node : _group.childNodes())
  {
    node->nodeGeometry().setHovered(true);
    node->nodeGraphicsObject().update();
  }
  update();
}

void
GroupGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  _areaRect.setFillColor();
  for(auto& node : _group.childNodes())
  {
    node->nodeGeometry().setHovered(false);
    node->nodeGraphicsObject().update();
  }
  update();
}

GroupGraphicsObject::
GroupGraphicsObject(QtNodes::FlowScene& scene, QtNodes::NodeGroup& nodeGroup)
  : _scene(scene)
  , _group(nodeGroup)
  , _areaRect(this)
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);

  setZValue(-1.0);

  setAcceptHoverEvents(true);
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

void
GroupGraphicsObject::
updateBounds()
{
//  if(!_group.childNodes().empty())
//  {
//    QRectF currentRect = _group.childNodes()[0]->nodeGraphicsObject().boundingRect();
//    QRectF rect = _group.childNodes()[0]->nodeGraphicsObject().boundingRect();
//    QPointF firstRectScenePos = _group.childNodes()[0]->nodeGraphicsObject().scenePos();
//    rect.translate(-firstRectScenePos);
//    QPointF rectScenePos;
//    for (auto& node : _group.childNodes())
//    {
//      currentRect = node->nodeGraphicsObject().boundingRect();
//      rectScenePos = node->nodeGraphicsObject().scenePos();
//      currentRect.translate(rectScenePos - firstRectScenePos);

//      rect = rect.united(currentRect);
////      rectScenePos = node->nodeGraphicsObject().scenePos();
////      if()
////      {

////      }
//    }
//    _areaRect.setRect(rect);
////    setPos(firstRectScenePos);
//  }
////  setPos(_areaRect.pos());
  update();
}

QRectF
GroupGraphicsObject::
boundingRect() const
{
  return _areaRect.boundingRect();
}

void
GroupGraphicsObject::
paint(QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget)
{
}
