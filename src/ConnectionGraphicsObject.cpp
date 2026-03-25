#include "ConnectionGraphicsObject.hpp"

#include "AbstractConnectionPainter.hpp"
#include "AbstractGraphModel.hpp"
#include "AbstractNodeGeometry.hpp"
#include "BasicGraphicsScene.hpp"
#include "ConnectionIdUtils.hpp"
#include "ConnectionState.hpp"
#include "ConnectionStyle.hpp"
#include "NodeConnectionInteraction.hpp"
#include "NodeGraphicsObject.hpp"
#include "StyleCollection.hpp"
#include "locateNode.hpp"

#include <QtGui/QPainterPath>
#include <QtGui/QPainterPathStroker>
#include <QtWidgets/QGraphicsBlurEffect>
#include <QtWidgets/QGraphicsDropShadowEffect>
#include <QtWidgets/QGraphicsSceneMouseEvent>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QStyleOptionGraphicsItem>

#include <QtCore/QDebug>

#include <stdexcept>

namespace QtNodes {

ConnectionGraphicsObject::ConnectionGraphicsObject(BasicGraphicsScene &scene,
                                                   ConnectionId const connectionId)
    : _connectionId(connectionId)
    , _graphModel(scene.graphModel())
    , _connectionState(*this)
    , _out{0, 0}
    , _in{0, 0}
{
    scene.addItem(this);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsFocusable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);

    setAcceptHoverEvents(true);

    setZValue(-1.0);

    initializePosition();
}

void ConnectionGraphicsObject::initializePosition()
{
    // This function is only called when the ConnectionGraphicsObject
    // is newly created. At this moment both end coordinates are (0, 0)
    // in Connection G.O. coordinates. The position of the whole
    // Connection G. O. in scene coordinate system is also (0, 0).
    // By moving the whole object to the Node Port position
    // we position both connection ends correctly.

    if (_connectionState.requiredPort() != PortType::None) {
        PortType attachedPort = oppositePort(_connectionState.requiredPort());

        PortIndex portIndex = getPortIndex(attachedPort, _connectionId);
        NodeId nodeId = getNodeId(attachedPort, _connectionId);

        NodeGraphicsObject *ngo = nodeScene()->nodeGraphicsObject(nodeId);

        if (ngo) {
            QTransform nodeSceneTransform = ngo->sceneTransform();

            AbstractNodeGeometry &geometry = nodeScene()->nodeGeometry();

            QPointF pos = geometry.portScenePosition(nodeId,
                                                     attachedPort,
                                                     portIndex,
                                                     nodeSceneTransform);

            this->setPos(pos);
        }
    }

    move();
}

AbstractGraphModel &ConnectionGraphicsObject::graphModel() const
{
    return _graphModel;
}

BasicGraphicsScene *ConnectionGraphicsObject::nodeScene() const
{
    auto *sceneObject = dynamic_cast<BasicGraphicsScene *>(scene());
    Q_ASSERT(sceneObject != nullptr);
    return sceneObject;
}

ConnectionId const &ConnectionGraphicsObject::connectionId() const noexcept
{
    return _connectionId;
}

void ConnectionGraphicsObject::rebuildCachedGeometry() const
{
    if (!_geometryDirty) {
        return;
    }
    _geometryDirty = false;

    // Cubic path
    auto const c1c2 = pointsC1C2();
    _cachedCubicPath = QPainterPath(_out);
    _cachedCubicPath.cubicTo(c1c2.first, c1c2.second, _in);

    for (int i = 0; i < k_path_sample_count; ++i) {
        double const ratio = double(i) / (k_path_sample_count - 1);
        _cachedSamplePoints[i] = _cachedCubicPath.pointAtPercent(ratio);
    }
    _cachedMidPoint = _cachedSamplePoints[k_path_sample_count / 2];

    // Bounding rect
    QRectF basicRect = QRectF(_out, _in).normalized();
    QRectF c1c2Rect = QRectF(c1c2.first, c1c2.second).normalized();
    QRectF commonRect = basicRect.united(c1c2Rect);

    auto const &connectionStyle = StyleCollection::connectionStyle();
    float const diam = connectionStyle.pointDiameter();
    QPointF const cornerOffset(diam, diam);
    commonRect.setTopLeft(commonRect.topLeft() - cornerOffset);
    commonRect.setBottomRight(commonRect.bottomRight() + 2 * cornerOffset);
    _cachedBoundingRect = commonRect;

    // Stroke path for hit testing
    QPainterPath linearized(_out);
    for (int i = 1; i < k_path_sample_count; ++i) {
        linearized.lineTo(_cachedSamplePoints[i]);
    }
    QPainterPathStroker stroker;
    stroker.setWidth(10.0);
    _cachedStrokePath = stroker.createStroke(linearized);
}

QRectF ConnectionGraphicsObject::boundingRect() const
{
    rebuildCachedGeometry();
    return _cachedBoundingRect;
}

QPainterPath ConnectionGraphicsObject::shape() const
{
    rebuildCachedGeometry();
    return _cachedStrokePath;
}

QPainterPath const &ConnectionGraphicsObject::cachedCubicPath() const
{
    rebuildCachedGeometry();
    return _cachedCubicPath;
}

QPainterPath const &ConnectionGraphicsObject::cachedStrokePath() const
{
    rebuildCachedGeometry();
    return _cachedStrokePath;
}

QPointF const &ConnectionGraphicsObject::cachedSamplePoint(int index) const
{
    rebuildCachedGeometry();
    Q_ASSERT(index >= 0 && index < k_path_sample_count);
    return _cachedSamplePoints[index];
}

QPointF const &ConnectionGraphicsObject::cachedMidPoint() const noexcept
{
    rebuildCachedGeometry();
    return _cachedMidPoint;
}

QPointF const &ConnectionGraphicsObject::endPoint(PortType portType) const
{
    Q_ASSERT(portType != PortType::None);

    return (portType == PortType::Out ? _out : _in);
}

void ConnectionGraphicsObject::setEndPoint(PortType portType, QPointF const &point)
{
    if (portType == PortType::In)
        _in = point;
    else
        _out = point;

    _geometryDirty = true;
}

void ConnectionGraphicsObject::move()
{
    QPointF newOut = _out;
    QPointF newIn = _in;

    auto moveEnd = [this](ConnectionId cId, PortType portType, QPointF &endPoint) {
        NodeId nodeId = getNodeId(portType, cId);

        if (nodeId == InvalidNodeId)
            return;

        NodeGraphicsObject *ngo = nodeScene()->nodeGraphicsObject(nodeId);

        if (ngo) {
            AbstractNodeGeometry &geometry = nodeScene()->nodeGeometry();

            QPointF scenePos = geometry.portScenePosition(nodeId,
                                                          portType,
                                                          getPortIndex(portType, cId),
                                                          ngo->sceneTransform());

            endPoint = sceneTransform().inverted().map(scenePos);
        }
    };

    moveEnd(_connectionId, PortType::Out, newOut);
    moveEnd(_connectionId, PortType::In, newIn);

    if (newOut == _out && newIn == _in) {
        return;
    }

    prepareGeometryChange();
    _out = newOut;
    _in = newIn;
    _geometryDirty = true;

    update();
}

ConnectionState const &ConnectionGraphicsObject::connectionState() const
{
    return _connectionState;
}

ConnectionState &ConnectionGraphicsObject::connectionState()
{
    return _connectionState;
}

void ConnectionGraphicsObject::paint(QPainter *painter,
                                     QStyleOptionGraphicsItem const *option,
                                     QWidget *)
{
    if (!scene())
        return;

    painter->setClipRect(option->exposedRect);

    nodeScene()->connectionPainter().paint(painter, *this);
}

void ConnectionGraphicsObject::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mousePressEvent(event);
}

void ConnectionGraphicsObject::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    prepareGeometryChange();

    auto view = static_cast<QGraphicsView *>(event->widget());
    auto ngo = locateNodeAt(event->scenePos(), *nodeScene(), view->transform());
    if (ngo) {
        ngo->reactToConnection(this);

        _connectionState.setLastHoveredNode(ngo->nodeId());
    } else {
        _connectionState.resetLastHoveredNode();
    }

    //-------------------

    auto requiredPort = _connectionState.requiredPort();

    if (requiredPort != PortType::None) {
        setEndPoint(requiredPort, event->pos());
    }

    //-------------------

    update();

    event->accept();
}

void ConnectionGraphicsObject::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsItem::mouseReleaseEvent(event);

    ungrabMouse();
    event->accept();

    auto view = static_cast<QGraphicsView *>(event->widget());

    Q_ASSERT(view);

    auto ngo = locateNodeAt(event->scenePos(), *nodeScene(), view->transform());

    bool wasConnected = false;

    if (ngo) {
        NodeConnectionInteraction interaction(*ngo, *this, *nodeScene());

        wasConnected = interaction.tryConnect();
    }

    // If connection attempt was unsuccessful
    if (!wasConnected) {
        // Resulting unique_ptr is not used and automatically deleted.
        nodeScene()->resetDraftConnection();
    }
}

void ConnectionGraphicsObject::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    _connectionState.setHovered(true);

    update();

    // Signal
    nodeScene()->connectionHovered(connectionId(), event->screenPos());

    event->accept();
}

void ConnectionGraphicsObject::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    _connectionState.setHovered(false);

    update();

    // Signal
    nodeScene()->connectionHoverLeft(connectionId());

    event->accept();
}

std::pair<QPointF, QPointF> ConnectionGraphicsObject::pointsC1C2() const
{
    return computeControlPoints(nodeScene()->orientation());
}

std::pair<QPointF, QPointF> ConnectionGraphicsObject::computeControlPoints(
    Qt::Orientation orientation) const
{
    double const defaultOffset = 200;

    // In horizontal mode the primary axis is X; in vertical mode it is Y.
    auto primary = [&](QPointF const &p) {
        return orientation == Qt::Horizontal ? p.x() : p.y();
    };
    auto secondary = [&](QPointF const &p) {
        return orientation == Qt::Horizontal ? p.y() : p.x();
    };

    double primaryDistance = primary(_in) - primary(_out);

    double primaryOffset = qMin(defaultOffset, std::abs(primaryDistance));

    double secondaryOffset = 0;

    double ratio = 0.5;

    if (primaryDistance <= 0) {
        double secDistance = secondary(_in) - secondary(_out) + 20;

        double vector = secDistance < 0 ? -1.0 : 1.0;

        secondaryOffset = qMin(defaultOffset, std::abs(secDistance)) * vector;

        ratio = 1.0;
    }

    primaryOffset *= ratio;

    double hOff, vOff;
    if (orientation == Qt::Horizontal) {
        hOff = primaryOffset;
        vOff = secondaryOffset;
    } else {
        hOff = secondaryOffset;
        vOff = primaryOffset;
    }

    QPointF c1(_out.x() + hOff, _out.y() + vOff);
    QPointF c2(_in.x() - hOff, _in.y() - vOff);

    return std::make_pair(c1, c2);
}

} // namespace QtNodes
