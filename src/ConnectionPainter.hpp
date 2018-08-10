#pragma once

#include <QtGui/QPainter>

namespace QtNodes
{

class ConnectionGeometry;
class ConnectionGraphicsObject;

class ConnectionPainter
{
public:

  static
  void
  paint(QPainter* painter,
        ConnectionGraphicsObject const& connection);

  static
  QPainterPath
  getPainterStroke(ConnectionGeometry const& geom);
};
}
