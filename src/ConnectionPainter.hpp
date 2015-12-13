#ifndef CONNECTION_PAINTER_HPP
#define CONNECTION_PAINTER_HPP

#include <QtGui/QPainter>

class ConnectionGeometry;

class ConnectionPainter
{
public:

  ConnectionPainter();

public:

  QPainterPath cubicPath(ConnectionGeometry const& geom) const;

  QPainterPath getPainterStroke(ConnectionGeometry const& geom) const;

  void paint(QPainter* painter,
             ConnectionGeometry const& geom) const;
};

#endif //  CONNECTION_PAINTER_HPP
