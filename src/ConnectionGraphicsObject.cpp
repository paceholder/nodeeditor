#include "ConnectionGraphicsObject.hpp"

#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QStyleOptionGraphicsItem>
#include <QtWidgets/QGraphicsView>

#include <QtCore/QDebug>

#include "ConnectionIdUtils.hpp"
#include "ConnectionPainter.hpp"
#include "ConnectionState.hpp"
#include "ConnectionStyle.hpp"
#include "GraphModel.hpp"
#include "NodeConnectionInteraction.hpp"
#include "NodeGeometry.hpp"
#include "NodeGraphicsObject.hpp"
#include "BasicGraphicsScene.hpp"
#include "StyleCollection.hpp"
#include "locateNode.hpp"


namespace QtNodes
{

ConnectionGraphicsObject::
ConnectionGraphicsObject(BasicGraphicsScene &scene,
                         ConnectionId const  connectionId)
  : _connectionId(connectionId)
  , _graphModel(scene.graphModel())
  , _connectionState(*this)
  , _out{0, 0}
  , _in{0, 0}
{
  scene.addItem(this);

  setFlag(QGraphicsItem::ItemIsMovable,    true);
  setFlag(QGraphicsItem::ItemIsFocusable,  true);
  setFlag(QGraphicsItem::ItemIsSelectable, true);

  setAcceptHoverEvents(true);

  //addGraphicsEffect();

  setZValue(-1.0);

  initializePosition();
}


void
ConnectionGraphicsObject::
initializePosition()
{
  // This function is only called when the ConnectionGraphicsObject
  // is newly created. At this moment both end coordinates are (0, 0)
  // in Connection G.O. coordinates. The position of the whole
  // Connection G. O. in scene coordinate system is also (0, 0).
  // By moving the whole object to the Node Port position
  // we position both connection ends correctly.

  if (_connectionState.requiredPort() != PortType::None)
  {
    PortType attachedPort = oppositePort(_connectionState.requiredPort());

    PortIndex portIndex = getPortIndex(attachedPort, _connectionId);
    NodeId nodeId       = getNodeId(attachedPort, _connectionId);

    NodeGraphicsObject * ngo =
      nodeScene()->nodeGraphicsObject(nodeId);

    QTransform nodeSceneTransform =
      ngo->sceneTransform();

    NodeGeometry geometry(*ngo);

    QPointF pos =
      geometry.portScenePosition(attachedPort,
                                 portIndex,
                                 nodeSceneTransform);

    this->setPos(pos);
  }

  move();
}


GraphModel &
ConnectionGraphicsObject::
graphModel() const
{
  return _graphModel;
}


BasicGraphicsScene *
ConnectionGraphicsObject::
nodeScene() const
{
  return dynamic_cast<BasicGraphicsScene*>(scene());
}


ConnectionId
ConnectionGraphicsObject::
connectionId() const
{
  return _connectionId;
}



QRectF
ConnectionGraphicsObject::
boundingRect() const
{
  auto points = pointsC1C2();

  // `normalized()` fixes inverted rects.
  QRectF basicRect = QRectF(_out, _in).normalized();

  QRectF c1c2Rect = QRectF(points.first, points.second).normalized();

  QRectF commonRect = basicRect.united(c1c2Rect);

  auto const &connectionStyle = StyleCollection::connectionStyle();
  float const diam = connectionStyle.pointDiameter();
  QPointF const cornerOffset(diam, diam);

  // Expand rect by port circle diameter
  commonRect.setTopLeft(commonRect.topLeft() - cornerOffset);
  commonRect.setBottomRight(commonRect.bottomRight() + 2 * cornerOffset);

  return commonRect;
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
  return ConnectionPainter::getPainterStroke(*this);
#endif
}


QPointF const &
ConnectionGraphicsObject::
endPoint(PortType portType) const
{
  Q_ASSERT(portType != PortType::None);

  return (portType == PortType::Out ?
          _out :
          _in);
}


void
ConnectionGraphicsObject::
setEndPoint(PortType portType, QPointF const &point)
{
  if (portType == PortType::In)
    _in = point;
  else
    _out = point;
}


void
ConnectionGraphicsObject::
setGeometryChanged()
{
  prepareGeometryChange();
}


void
ConnectionGraphicsObject::
move()
{
  auto moveEnd =
    [this](ConnectionId cId, PortType portType)
    {
      NodeId nodeId = getNodeId(portType, cId);

      if (nodeId == InvalidNodeId)
        return;

      NodeGraphicsObject * ngo =
        nodeScene()->nodeGraphicsObject(nodeId);

      NodeGeometry nodeGeometry(*ngo);

      QPointF scenePos =
        nodeGeometry.portScenePosition(portType,
                                       getPortIndex(portType, cId),
                                       ngo->sceneTransform());

      QPointF connectionPos = sceneTransform().inverted().map(scenePos);

      setEndPoint(portType, connectionPos);
    };

  moveEnd(_connectionId, PortType::Out);
  moveEnd(_connectionId, PortType::In);

  setGeometryChanged();

  update();
}


ConnectionState const &
ConnectionGraphicsObject::
connectionState() const
{
  return _connectionState;
}


ConnectionState &
ConnectionGraphicsObject::
connectionState()
{
  return _connectionState;
}


void
ConnectionGraphicsObject::
paint(QPainter * painter,
      QStyleOptionGraphicsItem const * option,
      QWidget *)
{
  if (!scene())
    return;

  painter->setClipRect(option->exposedRect);

  ConnectionPainter::paint(painter, *this);
}


void
ConnectionGraphicsObject::
mousePressEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem::mousePressEvent(event);
}


void
ConnectionGraphicsObject::
mouseMoveEvent(QGraphicsSceneMouseEvent * event)
{
  prepareGeometryChange();

  auto view = static_cast<QGraphicsView *>(event->widget());
  auto ngo  = locateNodeAt(event->scenePos(),
                           *nodeScene(),
                           view->transform());
  if (ngo)
  {
    _connectionState.interactWithNode(ngo->nodeId());

    PortType knownPortType = oppositePort(_connectionState.requiredPort());

    NodeId knownNodeId = (knownPortType == PortType::Out) ?
                         std::get<0>(_connectionId) :
                         std::get<2>(_connectionId);

    PortIndex knownPortIndex = (knownPortType ==  PortType::Out) ?
                               std::get<1>(_connectionId) :
                               std::get<3>(_connectionId);

    NodeDataType knownDataType =
      _graphModel.portData(knownNodeId,
                           knownPortType,
                           knownPortIndex,
                           PortRole::DataType).value<NodeDataType>();

    // Sets node's mouse dragging position in nodes coordinates.
    ngo->nodeState().reactToPossibleConnection(_connectionState.requiredPort(),
                                               knownDataType,
                                               event->scenePos());
  }

  //-------------------

  auto requiredPort = _connectionState.requiredPort();

  if (requiredPort != PortType::None)
  {
    setEndPoint(requiredPort, event->pos());
  }

  //-------------------

  update();

  event->accept();
}


void
ConnectionGraphicsObject::
mouseReleaseEvent(QGraphicsSceneMouseEvent * event)
{
  QGraphicsItem::mouseReleaseEvent(event);

  ungrabMouse();
  event->accept();

  auto view = static_cast<QGraphicsView *>(event->widget());

  Q_ASSERT(view);

  auto ngo = locateNodeAt(event->scenePos(),
                          *nodeScene(),
                          view->transform());

  if (ngo)
  {
    NodeConnectionInteraction interaction(*ngo, *this, *nodeScene());

    interaction.tryConnect();
  }

  // If connection attempt was unsuccessful
  if (_connectionState.requiresPort())
  {
    // Resulting unique_ptr is not used and automatically deleted.
    nodeScene()->resetDraftConnection();
  }
}


void
ConnectionGraphicsObject::
hoverEnterEvent(QGraphicsSceneHoverEvent * event)
{
  _connectionState.setHovered(true);

  update();

  // Signal
  nodeScene()->connectionHovered(connectionId(), event->screenPos());

  event->accept();
}


void
ConnectionGraphicsObject::
hoverLeaveEvent(QGraphicsSceneHoverEvent * event)
{
  _connectionState.setHovered(false);

  update();

  // Signal
  nodeScene()->connectionHoverLeft(connectionId());

  event->accept();
}


std::pair<QPointF, QPointF>
ConnectionGraphicsObject::
pointsC1C2() const
{
  const double defaultOffset = 200;

  double xDistance = _in.x() - _out.x();

  double horizontalOffset = qMin(defaultOffset, std::abs(xDistance));

  double verticalOffset = 0;

  double ratioX = 0.5;

  if (xDistance <= 0)
  {
    double yDistance = _in.y() - _out.y() + 20;

    double vector = yDistance < 0 ? -1.0 : 1.0;

    verticalOffset = qMin(defaultOffset, std::abs(yDistance)) * vector;

    ratioX = 1.0;
  }

  horizontalOffset *= ratioX;

  QPointF c1(_out.x() + horizontalOffset,
             _out.y() + verticalOffset);

  QPointF c2(_in.x() - horizontalOffset,
             _in.y() - verticalOffset);

  return std::make_pair(c1, c2);
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


}
