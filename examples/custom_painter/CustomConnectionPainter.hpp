#pragma once

#include <QtNodes/AbstractConnectionPainter>

#include <QPainter>
#include <QPainterPath>

namespace QtNodes {
class ConnectionGraphicsObject;
}

using QtNodes::AbstractConnectionPainter;
using QtNodes::ConnectionGraphicsObject;

/// Custom connection painter that draws dashed lines with arrows
class CustomConnectionPainter : public AbstractConnectionPainter
{
public:
    void paint(QPainter *painter, ConnectionGraphicsObject const &cgo) const override;
    QPainterPath getPainterStroke(ConnectionGraphicsObject const &cgo) const override;

private:
    QPainterPath createPath(ConnectionGraphicsObject const &cgo) const;
    void drawArrow(QPainter *painter, QPointF const &tip, QPointF const &from) const;
};
