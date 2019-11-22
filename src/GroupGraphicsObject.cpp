#include "GroupGraphicsObject.hpp"

#include <QGraphicsSceneMouseEvent>

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

void GroupGraphicsObject::updateBounds()
{
  if (!_group.childNodes().empty())
  {
    QRectF currentRect =
      _group.childNodes()[0]->nodeGraphicsObject().  boundingRect();
    QRectF rect = currentRect;
    QPointF firstRectScenePos =
      _group.childNodes()[0]->nodeGraphicsObject().scenePos();
    rect.translate(firstRectScenePos);
    QPointF finalRectScenePos = firstRectScenePos;
    QPointF rectScenePos;
    for (auto& node : _group.childNodes())
    {
      currentRect = node->nodeGraphicsObject().boundingRect();
      rectScenePos = node->nodeGraphicsObject().scenePos();
      currentRect.translate(rectScenePos);

      rect = rect.united(currentRect);

      finalRectScenePos.setX(std::min(finalRectScenePos.x(),
                                      rectScenePos.x()));
      finalRectScenePos.setY(std::min(finalRectScenePos.y(),
                                      rectScenePos.y()));
    }
//    rect.translate(-firstRectScenePos);
    setRect(rect);
//    setPos(firstRectScenePos);
    update();
  }
}

void GroupGraphicsObject::paint(QPainter* painter,
                                const QStyleOptionGraphicsItem* option,
                                QWidget* widget)
{
  painter->setBrush(_currentColor);
  painter->setPen(Qt::PenStyle::DashLine);
  painter->drawRoundedRect(rect(), roundedBorderRadius, roundedBorderRadius);
}

QRectF GroupGraphicsObject::boundingRect() const
{
  return rect();
}
