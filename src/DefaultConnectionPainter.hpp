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
    static QPolygonF createArrowPoly(const QPainterPath& p, double mRadius,double arrowSize,bool drawIn = true);
private:
    QPainterPath cubicPath(ConnectionGraphicsObject const &connection) const;
    void drawSketchLine(QPainter *painter, ConnectionGraphicsObject const &cgo,QPainterPath const & cubic) const;
    void drawHoveredOrSelected(QPainter *painter, ConnectionGraphicsObject const &cgo,QPainterPath const & cubic) const;
    void drawNormalLine(QPainter *painter, ConnectionGraphicsObject const &cgo,QPainterPath const & cubic) const;
#ifdef NODE_DEBUG_DRAWING
    void debugDrawing(QPainter *painter, ConnectionGraphicsObject const &cgo,QPainterPath const & cubic) const;
#endif
};

} // namespace QtNodes
