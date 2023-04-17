#pragma once

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

#include "AbstractConnectionPainter.hpp"
#include "Definitions.hpp"

namespace QtNodes {

class ConnectionGeometry;
class ConnectionGraphicsObject;

class ConnectionPainter : public AbstractConnectionPainter
{
public:
    void paint(QPainter *painter, ConnectionGraphicsObject const &cgo) const override;
    QPainterPath getPainterStroke(ConnectionGraphicsObject const &cgo) const override;

private:
    static QPainterPath cubicPath(ConnectionGraphicsObject const &connection);
    static void drawSketchLine(QPainter *painter, ConnectionGraphicsObject const &cgo);
    static void drawHoveredOrSelected(QPainter *painter, ConnectionGraphicsObject const &cgo);
    static void drawNormalLine(QPainter *painter, ConnectionGraphicsObject const &cgo);
};

} // namespace QtNodes
