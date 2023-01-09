#pragma once

#include <QtGui/QPainter>
#include <QtGui/QPainterPath>

#include "Definitions.hpp"

namespace QtNodes {

class ConnectionGeometry;
class ConnectionGraphicsObject;

class ConnectionPainter
{
public:
    static void paint(QPainter *painter, ConnectionGraphicsObject const &cgo);

    static QPainterPath getPainterStroke(ConnectionGraphicsObject const &cgo);
};

} // namespace QtNodes
