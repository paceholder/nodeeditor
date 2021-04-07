#include "GroupGraphicsObject.hpp"

#include <iostream>
#include <cstdlib>

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

#include "FlowScene.hpp"
#include "NodePainter.hpp"

#include "Group.hpp"
#include "NodeDataModel.hpp"
#include "NodeConnectionInteraction.hpp"

#include "StyleCollection.hpp"

using QtNodes::GroupGraphicsObject;
using QtNodes::Node;
using QtNodes::Group;
using QtNodes::FlowScene;

GroupGraphicsObject::
GroupGraphicsObject(FlowScene &scene, Group& group)
  : _scene(scene)
  , _group(group)
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

  setZValue(-2);

  r = g = b = 135;

  QGraphicsProxyWidget* _proxyWidget = new QGraphicsProxyWidget(this);
  
  QString name = group.GetName();
  QLineEdit* l = new QLineEdit(name);
  l->setAlignment(Qt::AlignHCenter);
  l->setStyleSheet("\
  QLineEdit {\
    border: 0px solid gray;\
    border-radius: 0px;\
    padding: 0 0px;\
    background: rgb(135, 135, 135);\
    selection-background-color: darkgray;\
    color: rgb(220, 220, 220);\
    font-size: 30px;\
  }\
  QLineEdit:hover {\
    background: rgb(90, 90, 90);\
  }\
  ");

  connect(l, &QLineEdit::textChanged, this, [&group](const QString &text) {
    group.SetName("wuh");
    std::cout << "HERE" << std::endl;
  });


  _proxyWidget->setWidget(l);
  _proxyWidget->setPos(pos() + QPointF(sizeX/2 - _proxyWidget->size().width()/2, 0));
}


GroupGraphicsObject::
~GroupGraphicsObject()
{
  _scene.removeItem(this);
}


Group&
GroupGraphicsObject::
group() {
  return _group;
}

Group const&
GroupGraphicsObject::
group() const {
  return _group;
}


QRectF
GroupGraphicsObject::
boundingRect() const
{
    return QRectF(0, 0, sizeX, sizeY);
}


void
GroupGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}

void
GroupGraphicsObject::
moveConnections() const {
  for(int i=0; i<childItems().size(); i++) {
    NodeGraphicsObject* ngo = dynamic_cast<NodeGraphicsObject*>(childItems()[i]);
    if (ngo) {
      ngo->moveConnections();
    }

    GroupGraphicsObject* ggo = dynamic_cast<GroupGraphicsObject*>(childItems()[i]);
    if (ggo) {
      ggo->moveConnections();
    }
  }
}



void
GroupGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* )
{
    painter->setClipRect(option->exposedRect);
    QRect rect(0, 0, sizeX, sizeY);
    auto color = QColor(r, g, b, 255);
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
  // deselect all other items after this one is selected
  if (!isSelected() &&
      !(event->modifiers() & Qt::ControlModifier))
  {
    _scene.clearSelection();
  }

  auto mousePos = event->pos();

  if (abs(mousePos.x() - sizeX) < 20 && abs(mousePos.y() - sizeY) < 20)
  {
    isResizingXY=true;
  } else  if (abs(mousePos.x() - sizeX) < 20)
  {
    isResizingX=true;
  } else  if (abs(mousePos.y() - sizeY) < 20) {
    isResizingY=true;
  }
}


void
GroupGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  if(isResizingX) {
    int diff = event->pos().x() - event->lastPos().x();
    prepareGeometryChange();
    sizeX += diff;
    update();
    // ngo->moveConnections();
    event->accept();
  }
  else if(isResizingY) {
    int diff = event->pos().y() - event->lastPos().y();
    prepareGeometryChange();
    sizeY += diff;
    update();
    // ngo->moveConnections();
    event->accept();
  } else if(isResizingXY) {
    auto diff = event->pos() - event->lastPos();
    prepareGeometryChange();
    sizeX += diff.x();
    sizeY += diff.y();
    update();
    // ngo->moveConnections();
    event->accept();    
  } else {
    _scene.groupMoved(_group, pos());
    QGraphicsObject::mouseMoveEvent(event);
    moveConnections();
  }
}


void
GroupGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsObject::mouseReleaseEvent(event);
  _scene.groupMoveFinished(_group, pos());

  isResizingX=false;
  isResizingY=false;
  isResizingXY=false;
}


void
GroupGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  auto mousePos    = event->pos();
  setCursor(QCursor());  
  update();
  event->accept();
}


void
GroupGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  // std::cout << "hverleave" << std::endl;
  update();
  event->accept();
}


void
GroupGraphicsObject::
hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  auto mousePos    = event->pos();

  if (abs(mousePos.x() - sizeX) < 20 && abs(mousePos.y() - sizeY) < 20)
  {
    setCursor(QCursor(Qt::SizeFDiagCursor));
  } else  if (abs(mousePos.x() - sizeX) < 20)
  {
    setCursor(QCursor(Qt::SizeHorCursor));
  } else  if (abs(mousePos.y() - sizeY) < 20) {
    setCursor(QCursor(Qt::SizeVerCursor));
  } else
  {
    setCursor(QCursor());
  }

  event->accept();
}


void
GroupGraphicsObject::
mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  _scene.groupDoubleClicked(group());
  // std::cout << "doubleclick" << std::endl;
  QGraphicsItem::mouseDoubleClickEvent(event);
}

void
GroupGraphicsObject::
contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
}
