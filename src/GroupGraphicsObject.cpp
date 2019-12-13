#include "GroupGraphicsObject.hpp"

#include <QGraphicsSceneMouseEvent>
#include <QStyleOptionGraphicsItem>
#include <nodes/Node>

#include "FlowScene.hpp"
#include "Connection.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::NodeGraphicsObject;
using QtNodes::NodeGroup;
using QtNodes::Connection;


PadlockGraphicsItem::
PadlockGraphicsItem(QGraphicsItem* parent)
  : QGraphicsPixmapItem(parent) {}

PadlockGraphicsItem::
PadlockGraphicsItem(const QPixmap& pixmap, QGraphicsItem* parent)
  : QGraphicsPixmapItem(pixmap, parent)
{
  _scaleFactor = _iconSize / pixmap.size().width();
  setScale(_scaleFactor);
}

GroupGraphicsObject::
GroupGraphicsObject(FlowScene& scene,
                    NodeGroup& nodeGroup)
  : _scene(scene)
  , _group(nodeGroup)
  , _possibleChild(nullptr)
{
  setRect(0, 0, _defaultWidth, _defaultHeight);

  _lockedGraphicsItem = new PadlockGraphicsItem(_lockedIcon, this);
  _unlockedGraphicsItem = new PadlockGraphicsItem(_unlockedIcon, this);

  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);

  _currentFillColor = kUnlockedFillColor;

  setZValue(-2.0);

  setAcceptHoverEvents(true);
}

GroupGraphicsObject::
~GroupGraphicsObject()
{
  _scene.removeItem(this);
}

void
GroupGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  setHovered(true);
}

void
GroupGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  setHovered(false);
}

void
GroupGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseMoveEvent(event);
  if (event->lastPos() != event->pos())
  {
    auto diff = event->pos() - event->lastPos();
    moveNodes(diff);
    moveConnections();
  }
}

void
GroupGraphicsObject::
mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);
  lock(!locked());
}

void
GroupGraphicsObject::
positionLockedIcon()
{
  _lockedGraphicsItem->setPos(boundingRect().topRight()
                              + QPointF(-(_roundedBorderRadius
                                          + _lockedGraphicsItem->boundingRect().width()
                                          * _lockedGraphicsItem->_scaleFactor),
                                        _roundedBorderRadius));
  _unlockedGraphicsItem->setPos(boundingRect().topRight()
                                + QPointF(-(_roundedBorderRadius
                                          + _unlockedGraphicsItem->boundingRect().width()
                                          * _unlockedGraphicsItem->_scaleFactor),
                                          _roundedBorderRadius));
}

void
GroupGraphicsObject::
setHovered(bool hovered)
{
  hovered?
  setFillColor(locked()? kLockedHoverColor : kUnlockedHoverColor) :
  setFillColor(locked()? kLockedFillColor : kUnlockedFillColor);

  for (auto& node : _group.childNodes())
  {
    node->nodeGeometry().setHovered(hovered);
    node->nodeGraphicsObject().update();
  }
  update();
}

void
GroupGraphicsObject::
setPossibleChild(QtNodes::NodeGraphicsObject* possibleChild)
{
  _possibleChild = possibleChild;
}

void
GroupGraphicsObject::
unsetPossibleChild()
{
  _possibleChild = nullptr;
}

std::vector<std::shared_ptr<Connection> >
GroupGraphicsObject::
connections() const
{
  return _scene.connectionsWithinGroup(group().id());
}

void
GroupGraphicsObject::
setPosition(const QPointF& position)
{
  QPointF diffPos = position - scenePos();
  moveNodes(diffPos);
  moveConnections();
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

bool
GroupGraphicsObject::
locked() const
{
  return _locked;
}

void
GroupGraphicsObject::
moveConnections()
{
  for (auto& node : group().childNodes())
  {
    node->nodeGraphicsObject().moveConnections();
  }
}

void
GroupGraphicsObject::
moveNodes(const QPointF& offset)
{
  for (auto& node : group().childNodes())
  {
    node->nodeGraphicsObject().moveBy(offset.x(), offset.y());
  }
}

void
GroupGraphicsObject::
lock(bool locked)
{
  for (auto& node : _group.childNodes())
  {
    node->nodeGraphicsObject().lock(locked);
  }
  _lockedGraphicsItem->setVisible(locked);
  _unlockedGraphicsItem->setVisible(!locked);
  setFillColor(locked? kLockedFillColor : kUnlockedFillColor);
  _locked = locked;
  setZValue(locked? 2.0 : -2.0);
}

void
GroupGraphicsObject::
paint(QPainter* painter,
      const QStyleOptionGraphicsItem* option,
      QWidget* widget)
{
  setRect(boundingRect());
  positionLockedIcon();
  painter->setClipRect(option->exposedRect);
  painter->setBrush(_currentFillColor);

  setBorderColor(isSelected()? kSelectedBorderColor : kUnselectedBorderColor);
  painter->setPen(QPen(_currentBorderColor, 1.0, Qt::PenStyle::DashLine));

  painter->drawRoundedRect(rect(), _roundedBorderRadius, _roundedBorderRadius);
}

QRectF
GroupGraphicsObject::
boundingRect() const
{
  QRectF ret{};
  for (auto& node : _group.childNodes())
  {
    NodeGraphicsObject* ngo = &node->nodeGraphicsObject();

    ret |= ngo->mapRectToScene(ngo->boundingRect());
  }
  if (_possibleChild)
  {
    ret |= _possibleChild->mapRectToScene(_possibleChild->boundingRect());
  }
  return mapRectFromScene(ret.marginsAdded(_margins));
}

void
GroupGraphicsObject::
setFillColor(const QColor& color)
{
  _currentFillColor = color;
  update();
}

void
GroupGraphicsObject::
setBorderColor(const QColor& color)
{
  _currentBorderColor = color;
  update();
}

void
GroupGraphicsObject::
resetSize()
{
  setRect(x(), y(), _defaultWidth, _defaultHeight);
}

