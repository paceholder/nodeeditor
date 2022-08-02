#include "NodeGraphicsObject.hpp"

#include <iostream>
#include <cstdlib>

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "GraphModel.hpp"
#include "NodeConnectionInteraction.hpp"
#include "NodeGeometry.hpp"
#include "NodePainter.hpp"
#include "StyleCollection.hpp"


namespace QtNodes
{

NodeGraphicsObject::
NodeGraphicsObject(BasicGraphicsScene &scene,
                   NodeId nodeId)
  : _nodeId(nodeId)
  , _graphModel(scene.graphModel())
  , _nodeState(*this)
  , _proxyWidget(nullptr)
{
  scene.addItem(this);

  setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
  setFlag(QGraphicsItem::ItemIsFocusable,                      true);
  setFlag(QGraphicsItem::ItemIsMovable,                        true);
  setFlag(QGraphicsItem::ItemIsSelectable,                     true);
  setFlag(QGraphicsItem::ItemSendsScenePositionChanges,        true);

  setCacheMode(QGraphicsItem::DeviceCoordinateCache);

  // TODO: Take style from model.
  auto const &nodeStyle = StyleCollection::nodeStyle();

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

  NodeGeometry geometry(*this);
  geometry.recalculateSize();


  QPointF const pos =
    _graphModel.nodeData(_nodeId, NodeRole::Position).value<QPointF>();

  setPos(pos);
}


GraphModel &
NodeGraphicsObject::
graphModel() const
{
  return _graphModel;
}


BasicGraphicsScene *
NodeGraphicsObject::
nodeScene() const
{
  return dynamic_cast<BasicGraphicsScene*>(scene());
}


void
NodeGraphicsObject::
embedQWidget()
{
  NodeGeometry geom(*this);

  if (auto w = _graphModel.nodeData(_nodeId,
                                    NodeRole::Widget).value<QWidget*>())
  {
    _proxyWidget = new QGraphicsProxyWidget(this);

    _proxyWidget->setWidget(w);

    _proxyWidget->setPreferredWidth(5);

    NodeGeometry(*this).recalculateSize();

    if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag)
    {
      // If the widget wants to use as much vertical space as possible, set
      // it to have the geom's equivalentWidgetHeight.
      _proxyWidget->setMinimumHeight(geom.equivalentWidgetHeight());
    }

    _proxyWidget->setPos(geom.widgetPosition());

    //update();

    _proxyWidget->setOpacity(1.0);
    _proxyWidget->setFlag(QGraphicsItem::ItemIgnoresParentOpacity);
  }
}


#if 0
void
NodeGraphicsObject::
onNodeSizeUpdated()
{
  if (nodeDataModel()->embeddedWidget())
  {
    nodeDataModel()->embeddedWidget()->adjustSize();
  }
  nodeGeometry().recalculateSize();
  for (PortType type: {PortType::In, PortType::Out})
  {
    for (auto & conn_set : nodeState().getEntries(type))
    {
      for (auto & pair: conn_set)
      {
        Connection* conn = pair.second;
        conn->getConnectionGraphicsObject().move();
      }
    }
  }
}


#endif


QRectF
NodeGraphicsObject::
boundingRect() const
{
  return NodeGeometry(*this).boundingRect();
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
  auto moveConns =
    [this](PortType portType, NodeRole nodeRole)
    {
      size_t const n =
        _graphModel.nodeData(_nodeId, nodeRole).toUInt();

      for (PortIndex portIndex = 0; portIndex < n; ++portIndex)
      {
        auto const &connectedNodes =
          _graphModel.connectedNodes(_nodeId, portType, portIndex);

        for (auto &cn: connectedNodes)
        {
          // out node id, out port index, in node id, in port index.
          ConnectionId connectionId =
            (portType == PortType::In) ?
            std::make_tuple(cn.first, cn.second, _nodeId, portIndex) :
            std::make_tuple(_nodeId, portIndex, cn.first, cn.second);

          auto cgo = nodeScene()->connectionGraphicsObject(connectionId);

          // TODO: Directly move the connection's end?
          cgo->move();
        }
      }
    };

  moveConns(PortType::In, NodeRole::NumberOfInPorts);
  moveConns(PortType::Out, NodeRole::NumberOfOutPorts);
}


void
NodeGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const * option,
      QWidget *)
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
  //if (_nodeState.locked())
  //return;

  BasicGraphicsScene * nodeScene = this->nodeScene();

  for (PortType portToCheck: {PortType::In, PortType::Out})
  {
    NodeGeometry nodeGeometry(*this);

    PortIndex const portIndex =
      nodeGeometry.checkHitScenePoint(portToCheck,
                                      event->scenePos(),
                                      sceneTransform());

    if (portIndex != InvalidPortIndex)
    {
      auto const &connectedNodes =
        _graphModel.connectedNodes(_nodeId, portToCheck, portIndex);

      // Start dragging existing connection.
      if (!connectedNodes.empty() && portToCheck == PortType::In)
      {
        auto const &cn = *connectedNodes.begin();

        // Need "reversed" connectin id if enabled for both port types.
        ConnectionId connectionId =
          std::make_tuple(cn.first, cn.second, _nodeId, portIndex);

        // Need ConnectionGraphicsObject

        NodeConnectionInteraction
          interaction(*this,
                      *nodeScene->connectionGraphicsObject(connectionId),
                      *nodeScene);

        interaction.disconnect(portToCheck);
      }
      else // initialize new Connection
      {
        if (portToCheck == PortType::Out)
        {
          auto const outPolicy =
            _graphModel.portData(_nodeId,
                                 portToCheck,
                                 portIndex,
                                 PortRole::ConnectionPolicy).value<ConnectionPolicy>();

          if (!connectedNodes.empty() &&
              outPolicy == ConnectionPolicy::One)
          {
            for (auto &cn : connectedNodes)
            {
              ConnectionId connectionId =
                std::make_tuple(_nodeId, portIndex, cn.first, cn.second);

              _graphModel.deleteConnection(connectionId);
            }
          }
        } // if port == out

        ConnectionId const incompleteConnectionId =
          makeIncompleteConnectionId(portToCheck, _nodeId, portIndex);

        nodeScene->makeDraftConnection(incompleteConnectionId);
      }
    }
  }

  if (_graphModel.nodeFlags(_nodeId) & NodeFlag::Resizable)
  {
    NodeGeometry geometry(*this);

    auto pos = event->pos();
    bool const hit = geometry.resizeRect().contains(QPoint(pos.x(), pos.y()));
    _nodeState.setResizing(hit);
  }
}


void
NodeGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  // deselect all other items after this one is selected
  if (!isSelected())
  {
    nodeScene()->clearSelection();
    setSelected(true);
  }

  if (_nodeState.resizing())
  {
    auto diff = event->pos() - event->lastPos();

    if (auto w = _graphModel.nodeData(_nodeId,
                                      NodeRole::Widget).value<QWidget*>())
    {
      prepareGeometryChange();

      auto oldSize = w->size();

      oldSize += QSize(diff.x(), diff.y());

      w->setFixedSize(oldSize);

      NodeGeometry geometry(*this);

      _proxyWidget->setMinimumSize(oldSize);
      _proxyWidget->setMaximumSize(oldSize);
      _proxyWidget->setPos(geometry.widgetPosition());

      geometry.recalculateSize();

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

  QRectF r = nodeScene()->sceneRect();

  r = r.united(mapToScene(boundingRect()).boundingRect());

  nodeScene()->setSceneRect(r);
}


void
NodeGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  _nodeState.setResizing(false);

  QGraphicsObject::mouseReleaseEvent(event);

  // position connections precisely after fast node move
  moveConnections();

  nodeScene()->nodeClicked(_nodeId);
}


void
NodeGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  // bring all the colliding nodes to background
  QList<QGraphicsItem *> overlapItems = collidingItems();

  for (QGraphicsItem * item : overlapItems)
  {
    if (item->zValue() > 0.0)
    {
      item->setZValue(0.0);
    }
  }

  // bring this node forward
  setZValue(1.0);

  _nodeState.setHovered(true);

  update();

  // Signal
  nodeScene()->nodeHovered(_nodeId, event->screenPos());

  event->accept();
}


void
NodeGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  _nodeState.setHovered(false);

  update();

  // Signal
  nodeScene()->nodeHoverLeft(_nodeId);

  event->accept();
}


void
NodeGraphicsObject::
hoverMoveEvent(QGraphicsSceneHoverEvent * event)
{
  auto pos = event->pos();

  NodeGeometry geometry(*this);

  if ((_graphModel.nodeFlags(_nodeId) | NodeFlag::Resizable) &&
      geometry.resizeRect().contains(QPoint(pos.x(), pos.y())))
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
mouseDoubleClickEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem::mouseDoubleClickEvent(event);

  nodeScene()->nodeDoubleClicked(_nodeId);
}


void
NodeGraphicsObject::
contextMenuEvent(QGraphicsSceneContextMenuEvent * event)
{
  nodeScene()->nodeContextMenu(_nodeId, mapToScene(event->pos()));
}


}
