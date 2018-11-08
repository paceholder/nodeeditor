#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsView>

#include "FlowScene.hpp"

#include "ConnectionGeometry.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionState.hpp"
#include "ConnectionBlurEffect.hpp"

#include "NodeGraphicsObject.hpp"

#include "NodeConnectionInteraction.hpp"

#include "Node.hpp"

using QtNodes::ConnectionGraphicsObject;
using QtNodes::Connection;
using QtNodes::ConnectionID;
using QtNodes::FlowScene;
using QtNodes::NodeDataType;
using QtNodes::NodeIndex;
using QtNodes::PortIndex;
using QtNodes::PortType;

ConnectionGraphicsObject::
ConnectionGraphicsObject(NodeIndex const& leftNode,
                         PortIndex leftPortIndex,
                         NodeIndex const& rightNode,
                         PortIndex rightPortIndex,
                         FlowScene&       scene)
  : _scene{scene}
  , _state(leftNode.isValid() ?
           (rightNode.isValid() ?  PortType::None : PortType::In) :
           PortType::Out)
  , _leftNode{leftNode}
  , _rightNode{rightNode}
  , _leftPortIndex{leftPortIndex}
  , _rightPortIndex{rightPortIndex}
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setAcceptHoverEvents(true);

  // addGraphicsEffect();

  setZValue(-1.0);

  // initialize the end points
  if (leftNode.isValid())
  {
    auto ngo = _scene.nodeGraphicsObject(leftNode);
    Q_ASSERT(ngo != nullptr);

    geometry().moveEndPoint(PortType::Out,  ngo->geometry().portScenePosition(leftPortIndex, PortType::Out, ngo->sceneTransform()));

  }
  if (rightNode.isValid())
  {
    auto ngo = _scene.nodeGraphicsObject(rightNode);
    Q_ASSERT(ngo != nullptr);

    geometry().moveEndPoint(PortType::In, ngo->geometry().portScenePosition(rightPortIndex, PortType::In, ngo->sceneTransform()));
  }

}
ConnectionGraphicsObject::
~ConnectionGraphicsObject()
{
  _scene.removeItem(this);
}


QRectF
ConnectionGraphicsObject::
boundingRect() const
{
  return _geometry.boundingRect();
}


QPainterPath
ConnectionGraphicsObject::
shape() const
{
#ifdef DEBUG_DRAWING

  //QPainterPath path;

  //path.addRect(boundingRect());
  //return path;

#else

  return ConnectionPainter::getPainterStroke(_geometry);

#endif
}


void
ConnectionGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}

ConnectionID
ConnectionGraphicsObject::
id() const
{
  ConnectionID ret;

  ret.lNodeID = _leftNode.id();
  ret.rNodeID = _rightNode.id();
  ret.lPortID = _leftPortIndex;
  ret.rPortID = _rightPortIndex;

  return ret;
}

NodeDataType
ConnectionGraphicsObject::
dataType(PortType ty) const
{
  // get a valid node
  auto n = node(ty);
  Q_ASSERT(n.isValid());

  return _scene.model()->nodePortDataType(n, portIndex(ty), ty);
}

void
ConnectionGraphicsObject::
move()
{
  for(PortType portType: { PortType::In, PortType::Out } )
  {
    auto nodeIndex = node(portType);
    if (nodeIndex.isValid())
    {
      auto const &nodeGraphics = *_scene.nodeGraphicsObject(nodeIndex);

      auto const &nodeGeom = nodeGraphics.geometry();

      QPointF scenePos =
        nodeGeom.portScenePosition(portIndex(portType),
                                   portType,
                                   nodeGraphics.sceneTransform());

      QTransform sceneTransform = this->sceneTransform();

      QPointF connectionPos = sceneTransform.inverted().map(scenePos);

      geometry().setEndPoint(portType,
                             connectionPos);

      setGeometryChanged();
      update();
    }
  }

}

void
ConnectionGraphicsObject::
lock(bool locked)
{
  setFlag(QGraphicsItem::ItemIsMovable, !locked);
  setFlag(QGraphicsItem::ItemIsFocusable, !locked);
  setFlag(QGraphicsItem::ItemIsSelectable, !locked);
}


void
ConnectionGraphicsObject::
paint(QPainter*                       painter,
      QStyleOptionGraphicsItem const* option,
      QWidget*)
{
  painter->setClipRect(option->exposedRect);

  ConnectionPainter::paint(painter,
                           *this);
}


void
ConnectionGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mousePressEvent(event);
  //event->ignore();
}


void
ConnectionGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  prepareGeometryChange();

  auto view = static_cast<QGraphicsView*>(event->widget());
  auto node = locateNodeAt(event->scenePos(),
                           _scene,
                           view->transform());

  state().interactWithNode(node);
  if (node)
  {
    node->reactToPossibleConnection(state().requiredPort(),
                                    dataType(oppositePort(state().requiredPort())),
                                    event->scenePos());
  }

  //-------------------

  QPointF offset = event->pos() - event->lastPos();

  auto requiredPort = _state.requiredPort();
  if (requiredPort != PortType::None)
  {
    _geometry.moveEndPoint(requiredPort, offset);
  }

  //-------------------

  update();

  event->accept();
}


void
ConnectionGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  ungrabMouse();
  event->accept();

  auto node = locateNodeAt(event->scenePos(), _scene,
                           _scene.views()[0]->transform());

  if (!node)
  {

    if (state().requiresPort())
    {
      Q_ASSERT(this == _scene._temporaryConn);
      // remove this from the scene
      _scene._temporaryConn = nullptr;
      deleteLater();
    }

    return;
  }

  if (!state().requiresPort())
  {
    return;
  }

  NodeConnectionInteraction interaction(node->index(), *this);

  if (node && interaction.tryConnect())
  {
    node->resetReactionToConnection();
    Q_ASSERT(this == _scene._temporaryConn);
    _scene._temporaryConn = nullptr;
    deleteLater();
  }
  else if (state().requiresPort())
  {
    Q_ASSERT(this == _scene._temporaryConn);
    _scene._temporaryConn = nullptr;
    deleteLater();
  }
}


void
ConnectionGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  geometry().setHovered(true);

  update();

  flowScene().model()->connectionHovered(_leftNode, _leftPortIndex, _rightNode, _rightPortIndex, event->screenPos(), true);

  event->accept();
}


void
ConnectionGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  geometry().setHovered(false);

  update();
  flowScene().model()->connectionHovered(_leftNode, _leftPortIndex, _rightNode, _rightPortIndex, event->screenPos(), false);
  event->accept();
}


void
ConnectionGraphicsObject::
addGraphicsEffect()
{
  auto effect = new QGraphicsBlurEffect;

  effect->setBlurRadius(5);
  setGraphicsEffect(effect);

  //auto effect = new QGraphicsDropShadowEffect;
  //auto effect = new ConnectionBlurEffect(this);
  //effect->setOffset(4, 4);
  //effect->setColor(QColor(Qt::gray).darker(800));
}
