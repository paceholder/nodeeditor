#pragma once

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

#include "AbstractConnectionPainter.hpp"
#include "Definitions.hpp"

namespace QtNodes {

class ConnectionGeometry;
class ConnectionGraphicsObject;

class DefaultConnectionPainter : public AbstractConnectionPainter
{
public:
    void paint(QPainter *painter, ConnectionGraphicsObject const &cgo) const override;
    QPainterPath getPainterStroke(ConnectionGraphicsObject const &cgo) const override;
private:
    QPainterPath cubicPath(ConnectionGraphicsObject const &connection);
    void drawSketchLine(QPainter *painter, ConnectionGraphicsObject const &cgo);
    void drawHoveredOrSelected(QPainter *painter, ConnectionGraphicsObject const &cgo);
    void drawNormalLine(QPainter *painter, ConnectionGraphicsObject const &cgo);
#ifdef NODE_DEBUG_DRAWING
    void debugDrawing(QPainter *painter, ConnectionGraphicsObject const &cgo);
#endif
};

} // namespace QtNodes
