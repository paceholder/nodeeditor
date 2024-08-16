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
};

} // namespace QtNodes
