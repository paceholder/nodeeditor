#ifndef CONNECTION_PAINTER_HPP
#define CONNECTION_PAINTER_HPP

#include <QtGui/QPainter>

class ConnectionGeometry;

class ConnectionPainter
{
public:

  ConnectionPainter();

public:

  static
  QPainterPath cubicPath(ConnectionGeometry const& geom);

  static
  QPainterPath getPainterStroke(ConnectionGeometry const& geom);

  static
  void paint(QPainter* painter,
             ConnectionGeometry const& geom);
};

#endif //  CONNECTION_PAINTER_HPP
