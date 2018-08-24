#include "NodeGraphicsObject.hpp"

#include <iostream>
#include <cstdlib>

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"

#include "FlowScene.hpp"
#include "NodePainter.hpp"

#include "NodeIndex.hpp"
#include "FlowSceneModel.hpp"
#include "NodeConnectionInteraction.hpp"

#include "StyleCollection.hpp"

using QtNodes::NodeGraphicsObject;
using QtNodes::Node;
using QtNodes::FlowScene;
using QtNodes::NodeIndex;
using QtNodes::NodeGeometry;
using QtNodes::NodeState;
using QtNodes::ConnectionPolicy;
using QtNodes::PortType;
using QtNodes::NodeDataType;

NodeGraphicsObject::
NodeGraphicsObject(FlowScene &scene,
                   NodeIndex const& node)
  : _scene(scene)
  , _nodeIndex(node)
  , _geometry(node)
  , _state(node)
  , _locked(false)
  , _proxyWidget(nullptr)
{
  _scene.addItem(this);

  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges, true);

  setCacheMode( QGraphicsItem::DeviceCoordinateCache );

  auto const &nodeStyle = flowScene().model()->nodeStyle(index());

  {
    auto effect = new QGraphicsDropShadowEffect;
    effect->setOffset(4, 4);
    effect->setBlurRadius(20);
    effect->setColor(nodeStyle.ShadowColor);

    setGraphicsEffect(effect);
  }

  setOpacity(nodeStyle.Opacity);

  setAcceptHoverEvents(true);

  setZValue(0);

  embedQWidget();

  // connect to the move signals
  auto onMoveSlot = [this] {
                      // ask the model to move it
                      if (!flowScene().model()->moveNode(index(), pos()))
                      {
                        // set the location back
                        setPos(flowScene().model()->nodeLocation(index()));
                        moveConnections();
                      }
                    };
  connect(this, &QGraphicsObject::xChanged, this, onMoveSlot);
  connect(this, &QGraphicsObject::yChanged, this, onMoveSlot);

}


NodeGraphicsObject::
~NodeGraphicsObject()
{
  if (_proxyWidget)
  {
    delete _proxyWidget->widget();
  }
  _scene.removeItem(this);
}
NodeIndex
NodeGraphicsObject::
index() const
{
  return _nodeIndex;
}


FlowScene&
NodeGraphicsObject::
flowScene()
{
  return _scene;
}

FlowScene const&
NodeGraphicsObject::
flowScene() const
{
  return _scene;
}

NodeGeometry&
NodeGraphicsObject::
geometry()
{
  return _geometry;
}

NodeGeometry const&
NodeGraphicsObject::
geometry() const
{
  return _geometry;
}


NodeState&
NodeGraphicsObject::
nodeState()
{
  return _state;
}

NodeState const&
NodeGraphicsObject::
nodeState() const
{
  return _state;
}

void
NodeGraphicsObject::
embedQWidget()
{

  if (auto w = flowScene().model()->nodeWidget(index()))
  {
    _proxyWidget = new QGraphicsProxyWidget(this);

    _proxyWidget->setWidget(w);

    _proxyWidget->setPreferredWidth(5);

    geometry().recalculateSize();

    _proxyWidget->setPos(geometry().widgetPosition());

    update();

    _proxyWidget->setOpacity(1.0);
    _proxyWidget->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
  }
}


QRectF
NodeGraphicsObject::
boundingRect() const
{
  return geometry().boundingRect();
}


void
NodeGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}


void
NodeGraphicsObject::
moveConnections() const
{
  for(PortType portType: {PortType::In, PortType::Out})
  {
    auto const & connectionEntries =
      nodeState().getEntries(portType);

    for (auto const & connections : connectionEntries)
    {
      for (auto & con : connections)
        con->move();
    }
  }
  ;
}



void
NodeGraphicsObject::
reactToPossibleConnection(PortType reactingPortType,

                          NodeDataType reactingDataType,
                          QPointF const &scenePoint)
{
  QTransform const t = sceneTransform();

  QPointF p = t.inverted().map(scenePoint);

  geometry().setDraggingPosition(p);

  update();

  _state.setReaction(NodeState::REACTING,
                     reactingPortType,
                     reactingDataType);
}


void
NodeGraphicsObject::
resetReactionToConnection()
{
  _state.setReaction(NodeState::NOT_REACTING);
  update();
}

void
NodeGraphicsObject::
lock(bool locked)
{
  _locked = locked;
  setFlag(QGraphicsItem::ItemIsMovable, !locked);
  setFlag(QGraphicsItem::ItemIsFocusable, !locked);
  setFlag(QGraphicsItem::ItemIsSelectable, !locked);
}


void
NodeGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const* option,
      QWidget* )
{
  painter->setClipRect(option->exposedRect);

  NodePainter::paint(painter, *this);
}


QVariant
NodeGraphicsObject::
itemChange(GraphicsItemChange change, const QVariant &value)
{
  if (change == ItemPositionChange && scene())
  {
    moveConnections();
  }

  return QGraphicsItem::itemChange(change, value);
}


void
NodeGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  if(_locked)
    return;

  // deselect all other items after this one is selected
  if (!isSelected() &&
      !(event->modifiers() & Qt::ControlModifier))
  {
    _scene.clearSelection();
  }

  for(PortType portToCheck: {PortType::In, PortType::Out})
  {
    // TODO do not pass sceneTransform
    int portIndex = geometry().checkHitScenePoint(portToCheck,
                                                  event->scenePos(),
                                                  sceneTransform());
    if (portIndex != INVALID)
    {
      std::vector<ConnectionGraphicsObject*> connections =
        nodeState().connections(portToCheck, portIndex);

      // start dragging existing connection
      if (!connections.empty() &&
          flowScene().model()->nodePortConnectionPolicy(index(), portIndex, portToCheck) == ConnectionPolicy::One)
      {
        Q_ASSERT(connections.size() == 1);

        auto con = *connections.begin();

        // remove it
        flowScene().model()->removeConnection(
          con->node(PortType::Out),
          con->portIndex(PortType::Out),
          con->node(PortType::In),
          con->portIndex(PortType::In));

        // start connecting anew, except start with the port that this connection was already connected to
        Q_ASSERT(_scene._temporaryConn == nullptr);
        if (portToCheck == PortType::In)
        {
          _scene._temporaryConn = new ConnectionGraphicsObject(con->node(PortType::Out), con->portIndex(PortType::Out), NodeIndex{}, -1, _scene);
          _scene._temporaryConn->geometry().setEndPoint(PortType::In, event->scenePos());
        }
        else {
          _scene._temporaryConn = new ConnectionGraphicsObject(NodeIndex{}, -1, con->node(PortType::In), con->portIndex(PortType::In), _scene);
          _scene._temporaryConn->geometry().setEndPoint(PortType::Out, event->scenePos());
        }
        _scene._temporaryConn->grabMouse();
      }
      else // initialize new Connection
      {
        if (portToCheck == PortType::In)
        {
          Q_ASSERT(_scene._temporaryConn == nullptr);
          _scene._temporaryConn = new ConnectionGraphicsObject(NodeIndex{}, -1, _nodeIndex, portIndex, _scene);
          _scene._temporaryConn->geometry().setEndPoint(PortType::Out, event->scenePos());
        }
        else {
          Q_ASSERT(_scene._temporaryConn == nullptr);
          _scene._temporaryConn = new ConnectionGraphicsObject(_nodeIndex, portIndex, NodeIndex{}, -1, _scene);
          _scene._temporaryConn->geometry().setEndPoint(PortType::In, event->scenePos());
        }

        _scene._temporaryConn->grabMouse();
      }
    }
  }

  auto pos = event->pos();

  if (flowScene().model()->nodeResizable(index()) &&
      geometry().resizeRect().contains(QPoint(pos.x(), pos.y())))
  {
    nodeState().setResizing(true);
  }
}


void
NodeGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{

  if (nodeState().resizing())
  {
    auto diff = event->pos() - event->lastPos();

    if (auto w = flowScene().model()->nodeWidget(index()))
    {
      prepareGeometryChange();

      auto oldSize = w->size();

      oldSize += QSize(diff.x(), diff.y());

      w->setFixedSize(oldSize);

      _proxyWidget->setMinimumSize(oldSize);
      _proxyWidget->setMaximumSize(oldSize);
      _proxyWidget->setPos(geometry().widgetPosition());

      geometry().recalculateSize();
      update();

      moveConnections();

      event->accept();
    }
  }
  else
  {
    QGraphicsObject::mouseMoveEvent(event);

    if (event->lastPos() != event->pos())
      moveConnections();

    event->ignore();
  }

  QRectF r = scene()->sceneRect();

  r = r.united(mapToScene(boundingRect()).boundingRect());

  scene()->setSceneRect(r);
}


void
NodeGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent* event)
{
  nodeState().setResizing(false);

  QGraphicsObject::mouseReleaseEvent(event);

  // position connections precisely after fast node move
  moveConnections();
}


void
NodeGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  // bring all the colliding nodes to background
  QList<QGraphicsItem *> overlapItems = collidingItems();

  for (QGraphicsItem *item : overlapItems)
  {
    if (item->zValue() > 0.0)
    {
      item->setZValue(0.0);
    }
  }
  // bring this node forward
  setZValue(1.0);

  geometry().setHovered(true);
  update();
  flowScene().model()->nodeHovered(index(), event->screenPos(), true);
  event->accept();
}


void
NodeGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  geometry().setHovered(false);
  update();
  flowScene().model()->nodeHovered(index(), event->screenPos(), false);
  event->accept();
}

void
NodeGraphicsObject::
hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  auto pos = event->pos();

  if (flowScene().model()->nodeResizable(index()) &&
      geometry().resizeRect().contains(QPoint(pos.x(), pos.y())))
  {
    setCursor(QCursor(Qt::SizeFDiagCursor));
  }
  else
  {
    setCursor(QCursor());
  }

  event->accept();
}


void
NodeGraphicsObject::
mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);

  flowScene().model()->nodeDoubleClicked(index(), event->screenPos());
}

void
NodeGraphicsObject::
contextMenuEvent(QGraphicsSceneContextMenuEvent* event)
{
  QGraphicsItem::contextMenuEvent(event);

  flowScene().model()->nodeContextMenu(index(), event->screenPos());
}
