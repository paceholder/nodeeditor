#pragma once

#include <QPainter>

#include "Export.hpp"

class QPainter;

namespace QtNodes {

class ConnectionGraphicsObject;

/// Class enables custom painting for connections.
class NODE_EDITOR_PUBLIC AbstractConnectionPainter
{
public:
    virtual ~AbstractConnectionPainter() = default;

    /**
     * Reimplement this function in order to have a custom connection painting.
     */
    virtual void paint(QPainter *painter, ConnectionGraphicsObject const &cgo) const = 0;

    virtual QPainterPath getPainterStroke(ConnectionGraphicsObject const &cgo) const = 0;
};
} // namespace QtNodes
