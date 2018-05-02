#pragma once

#include <QtGui/QPainter>

namespace QtNodes
{

class ConnectionGeometry;
class ConnectionState;
class Connection;

class ConnectionPainter
{
public:

  static
  void
  paint(QPainter* painter,
        Connection const& connection);

  static
  QPainterPath
  getPainterStroke(ConnectionGeometry const& geom);
};
}
