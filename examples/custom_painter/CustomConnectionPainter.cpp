#include "CustomConnectionPainter.hpp"

#include <QtNodes/ConnectionIdUtils>
#include <QtNodes/internal/ConnectionGraphicsObject.hpp>

#include <cmath>

using QtNodes::ConnectionId;

void CustomConnectionPainter::paint(QPainter *painter, ConnectionGraphicsObject const &cgo) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    QPainterPath path = createPath(cgo);

    // Draw the connection with a custom style
    QPen pen;

    if (cgo.isSelected()) {
        pen.setColor(QColor(255, 215, 0)); // Gold when selected
        pen.setWidth(4);
        pen.setStyle(Qt::SolidLine);
    } else if (cgo.connectionId().inNodeId == QtNodes::InvalidNodeId
               || cgo.connectionId().outNodeId == QtNodes::InvalidNodeId) {
        // Draft connection (being drawn)
        pen.setColor(QColor(150, 150, 150));
        pen.setWidth(2);
        pen.setStyle(Qt::DashLine);
    } else {
        // Normal connection - gradient-like effect using dashed line
        pen.setColor(QColor(0, 191, 255)); // Deep sky blue
        pen.setWidth(3);
        pen.setStyle(Qt::SolidLine);
    }

    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path);

    // Draw arrow at the end point (input port)
    if (cgo.connectionId().inNodeId != QtNodes::InvalidNodeId) {
        QPointF endPoint = cgo.endPoint(QtNodes::PortType::In);
        QPointF startPoint = cgo.endPoint(QtNodes::PortType::Out);

        // Get a point slightly before the end for arrow direction
        qreal t = 0.95;
        QPointF controlPoint1 = startPoint + QPointF(50, 0);
        QPointF controlPoint2 = endPoint - QPointF(50, 0);

        // Calculate point on bezier curve at t
        qreal mt = 1.0 - t;
        QPointF beforeEnd = mt * mt * mt * startPoint + 3.0 * mt * mt * t * controlPoint1
                            + 3.0 * mt * t * t * controlPoint2 + t * t * t * endPoint;

        drawArrow(painter, endPoint, beforeEnd);
    }
}

QPainterPath CustomConnectionPainter::getPainterStroke(ConnectionGraphicsObject const &cgo) const
{
    QPainterPath path = createPath(cgo);

    QPainterPathStroker stroker;
    stroker.setWidth(10.0);
    return stroker.createStroke(path);
}

QPainterPath CustomConnectionPainter::createPath(ConnectionGraphicsObject const &cgo) const
{
    QPointF const &startPoint = cgo.endPoint(QtNodes::PortType::Out);
    QPointF const &endPoint = cgo.endPoint(QtNodes::PortType::In);

    // Create a bezier curve
    QPainterPath path;
    path.moveTo(startPoint);

    // Control points for smooth curve
    qreal dx = std::abs(endPoint.x() - startPoint.x());
    qreal controlOffset = std::max(dx * 0.5, 30.0);

    QPointF controlPoint1(startPoint.x() + controlOffset, startPoint.y());
    QPointF controlPoint2(endPoint.x() - controlOffset, endPoint.y());

    path.cubicTo(controlPoint1, controlPoint2, endPoint);

    return path;
}

void CustomConnectionPainter::drawArrow(QPainter *painter, QPointF const &tip, QPointF const &from) const
{
    // Calculate arrow direction
    QPointF direction = tip - from;
    qreal length = std::sqrt(direction.x() * direction.x() + direction.y() * direction.y());

    if (length < 1.0)
        return;

    direction /= length;

    // Arrow parameters
    qreal arrowSize = 10.0;
    qreal arrowAngle = M_PI / 6.0; // 30 degrees

    // Calculate arrow points
    QPointF arrowP1 = tip
                      - arrowSize
                            * QPointF(direction.x() * std::cos(arrowAngle)
                                          - direction.y() * std::sin(arrowAngle),
                                      direction.x() * std::sin(arrowAngle)
                                          + direction.y() * std::cos(arrowAngle));

    QPointF arrowP2 = tip
                      - arrowSize
                            * QPointF(direction.x() * std::cos(-arrowAngle)
                                          - direction.y() * std::sin(-arrowAngle),
                                      direction.x() * std::sin(-arrowAngle)
                                          + direction.y() * std::cos(-arrowAngle));

    // Draw filled arrow
    QPainterPath arrowPath;
    arrowPath.moveTo(tip);
    arrowPath.lineTo(arrowP1);
    arrowPath.lineTo(arrowP2);
    arrowPath.closeSubpath();

    painter->fillPath(arrowPath, painter->pen().color());
}
