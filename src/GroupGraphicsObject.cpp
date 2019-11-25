#include "GroupGraphicsObject.hpp"

#include <QGraphicsSceneMouseEvent>
#include <nodes/Node>

#include "FlowScene.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::NodeGroup;

GroupGraphicsObject::GroupGraphicsObject(QtNodes::FlowScene& scene,
    QtNodes::NodeGroup& nodeGroup)
  : _scene(scene), _group(nodeGroup)
{
  setRect(0, 0, _defaultWidth, _defaultHeight);

  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);

  _currentColor = _fillColor;

  setZValue(-1.0);

  setAcceptHoverEvents(true);
}

void GroupGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  setHoverColor();
  for (auto& node : _group.childNodes())
  {
    node->nodeGeometry().setHovered(true);
    node->nodeGraphicsObject().update();
  }
  update();
}

void GroupGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  setFillColor();
  for (auto& node : _group.childNodes())
  {
    node->nodeGeometry().setHovered(false);
    node->nodeGraphicsObject().update();
  }
  update();
}

void GroupGraphicsObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);
  group().lock(!group().locked());
}

GroupGraphicsObject::~GroupGraphicsObject()
{
  _scene.removeItem(this);
}

NodeGroup& GroupGraphicsObject::group()
{
  return _group;
}

NodeGroup const& GroupGraphicsObject::group() const
{
  return _group;
}

void GroupGraphicsObject::addObject(NodeGraphicsObject& object)
{
  QMarginsF groupMargins{_groupBorderX, _groupBorderY, _groupBorderX, _groupBorderY};

  QPointF objScenePos = object.scenePos();
  object.setParentItem(this);
  object.setPos(mapFromScene(objScenePos));

  setRect(childrenBoundingRect().marginsAdded(groupMargins));

  update();
}

void GroupGraphicsObject::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
  painter->setBrush(_currentColor);
  painter->setPen(Qt::PenStyle::DashLine);
  painter->drawRoundedRect(rect(), _roundedBorderRadius, _roundedBorderRadius);
}

QRectF GroupGraphicsObject::boundingRect() const
{
  return rect();
}
