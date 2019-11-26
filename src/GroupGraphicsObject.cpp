#include "GroupGraphicsObject.hpp"

#include <QGraphicsSceneMouseEvent>
#include <nodes/Node>

#include "FlowScene.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeGroup;

GroupGraphicsObject::GroupGraphicsObject(QtNodes::FlowScene& scene,
    QtNodes::NodeGroup& nodeGroup)
  : _scene(scene), _group(nodeGroup)
{
  setRect(0, 0, _defaultWidth, _defaultHeight);

  _lockedGraphicsItem = new QGraphicsPixmapItem(_lockedIcon, this);
  _unlockedGraphicsItem = new QGraphicsPixmapItem(_unlockedIcon, this);

  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
//  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);

  _currentColor = kUnlockedFillColor;

  setZValue(-1.0);

  setAcceptHoverEvents(true);
}

void GroupGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  if (group().locked())
  {
    setColor(kLockedHoverColor);
  }
  else
  {
    setColor(kUnlockedHoverColor);
  }
  for (auto& node : _group.childNodes())
  {
    node->nodeGeometry().setHovered(true);
    node->nodeGraphicsObject().update();
  }
  update();
}

void GroupGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  if (group().locked())
  {
    setColor(kLockedFillColor);
  }
  else
  {
    setColor(kUnlockedFillColor);
  }
  for (auto& node : _group.childNodes())
  {
    node->nodeGeometry().setHovered(false);
    node->nodeGraphicsObject().update();
  }
  update();
}

void GroupGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseMoveEvent(event);
  if (event->lastPos() != event->pos())
  {
    moveConnections();
  }
}

void GroupGraphicsObject::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);
  group().lock(!group().locked());
}

void GroupGraphicsObject::positionLockedIcon()
{
  _lockedGraphicsItem->setPos(boundingRect().topRight()
                              + QPointF(-(_roundedBorderRadius +
                                          _lockedGraphicsItem->boundingRect().width()),
                                        _roundedBorderRadius));
  _unlockedGraphicsItem->setPos(boundingRect().topRight()
                                + QPointF(-(_roundedBorderRadius +
                                          _unlockedGraphicsItem->boundingRect().width()),
                                          _roundedBorderRadius));
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
  QPointF objScenePos = object.scenePos();
  object.setParentItem(this);
  object.setPos(mapFromScene(objScenePos));
  update();
}

void GroupGraphicsObject::moveConnections()
{
  for (auto& node : group().childNodes())
  {
    node->nodeGraphicsObject().moveConnections();
  }
}

void GroupGraphicsObject::lock(bool locked)
{
  _lockedGraphicsItem->setVisible(locked);
  _unlockedGraphicsItem->setVisible(!locked);
  setColor(locked? kLockedFillColor : kUnlockedFillColor);
}

void GroupGraphicsObject::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
  setRect(boundingRect());
  positionLockedIcon();
  painter->setBrush(_currentColor);
  painter->setPen(Qt::PenStyle::DashLine);
  painter->drawRoundedRect(rect(), _roundedBorderRadius, _roundedBorderRadius);
}

QRectF GroupGraphicsObject::boundingRect() const
{
  return childrenBoundingRect().marginsAdded(_margins);
}

void GroupGraphicsObject::setColor(const QColor& color)
{
  _currentColor = color;
  update();
}

void GroupGraphicsObject::resetSize()
{
  setRect(x(), y(), _defaultWidth, _defaultHeight);
}
