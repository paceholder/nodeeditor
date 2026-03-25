#pragma once

#include <array>
#include <utility>

#include <QtCore/QUuid>
#include <QtGui/QPainterPath>
#include <QtWidgets/QGraphicsObject>

#include "ConnectionState.hpp"
#include "Definitions.hpp"

class QGraphicsSceneMouseEvent;

namespace QtNodes {

class AbstractGraphModel;
class BasicGraphicsScene;

/// Graphic Object for connection. Adds itself to scene
class NODE_EDITOR_PUBLIC ConnectionGraphicsObject : public QGraphicsObject
{
    Q_OBJECT
public:
    // Needed for qgraphicsitem_cast
    enum { Type = UserType + 2 };

    int type() const noexcept override { return Type; }

public:
    ConnectionGraphicsObject(BasicGraphicsScene &scene, ConnectionId const connectionId);

    ~ConnectionGraphicsObject() = default;

public:
    AbstractGraphModel &graphModel() const;

    BasicGraphicsScene *nodeScene() const;

    ConnectionId const &connectionId() const noexcept;

    QRectF boundingRect() const override;

    QPainterPath shape() const override;

    QPointF const &endPoint(PortType portType) const;

    QPointF out() const noexcept { return _out; }

    QPointF in() const noexcept { return _in; }

    std::pair<QPointF, QPointF> pointsC1C2() const;

    /// Cached cubic bezier path, rebuilt only when endpoints change.
    QPainterPath const &cachedCubicPath() const;

    /// Cached painter stroke for hit testing, rebuilt only when endpoints change.
    QPainterPath const &cachedStrokePath() const;

    int cachedSamplePointCount() const noexcept { return k_path_sample_count; }

    QPointF const &cachedSamplePoint(int index) const;

    QPointF const &cachedMidPoint() const noexcept;

    void setEndPoint(PortType portType, QPointF const &point);

    /// Updates the position of both ends
    void move();

    ConnectionState const &connectionState() const;

    ConnectionState &connectionState();

protected:
    void paint(QPainter *painter,
               QStyleOptionGraphicsItem const *option,
               QWidget *widget = 0) override;

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;

    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) override;

    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;

    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;

private:
    void initializePosition();

    std::pair<QPointF, QPointF> computeControlPoints(Qt::Orientation orientation) const;

    void rebuildCachedGeometry() const;
    static constexpr int k_path_sample_count = 61;

    ConnectionId _connectionId;

    AbstractGraphModel &_graphModel;

    ConnectionState _connectionState;

    mutable QPointF _out;
    mutable QPointF _in;

    mutable bool _geometryDirty = true;
    mutable QRectF _cachedBoundingRect;
    mutable QPainterPath _cachedCubicPath;
    mutable QPainterPath _cachedStrokePath;
    mutable std::array<QPointF, k_path_sample_count> _cachedSamplePoints{};
    mutable QPointF _cachedMidPoint;
};

} // namespace QtNodes
