#include "GroupGraphicsObject.hpp"

#include <iostream>
#include <cstdlib>

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

#include "FlowScene.hpp"
#include "NodePainter.hpp"

#include "Node.hpp"
#include "NodeDataModel.hpp"
#include "NodeConnectionInteraction.hpp"

#include "StyleCollection.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::Node;
using QtNodes::FlowScene;

GroupGraphicsObject::
GroupGraphicsObject(FlowScene &scene)
  : _scene(scene)
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

  setCacheMode( QGraphicsItem::DeviceCoordinateCache );

  setAcceptHoverEvents(true);
  setAcceptTouchEvents(true);

  setZValue(0);
}


GroupGraphicsObject::
~GroupGraphicsObject()
{
  _scene.removeItem(this);
}


QRectF
GroupGraphicsObject::
boundingRect() const
{
    return QRectF(0, 0, 1000, 1000);
}


void
GroupGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}



void
GroupGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* )
{
    painter->setClipRect(option->exposedRect);
    QRect rect(0, 0, 1000, 1000);
    auto color = QColor(255, 0, 0, 255);
    painter->drawRect(rect);
    painter->fillRect(rect, QBrush(color));
}


QVariant
GroupGraphicsObject::
itemChange(GraphicsItemChange change, const QVariant &value)
{
  return QGraphicsItem::itemChange(change, value);
}


void
GroupGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent * event)
{
}


void
GroupGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
    // setPos(mapFromScene(event->scenePos()));
    QGraphicsObject::mouseMoveEvent(event);
    // std::cout << scene()->scale() << std::endl;
    // event->ignore();
    // //DO SNAPPING HERE
    // QRectF r = scene()->sceneRect();
    // r = r.united(mapToScene(boundingRect()).boundingRect());
    // scene()->setSceneRect(r);
}


void
GroupGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsObject::mouseReleaseEvent(event);
}


void
GroupGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  update();
  event->accept();
}


void
GroupGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  update();
  event->accept();
}


void
GroupGraphicsObject::
hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  event->accept();
}


void
GroupGraphicsObject::
mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);
}

void
GroupGraphicsObject::
contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
}
