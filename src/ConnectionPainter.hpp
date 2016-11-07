#pragma once

#include <memory>
#include <QtGui/QPainter>

class ConnectionGeometry;
class ConnectionState;
class Connection;

class ConnectionPainter
{
public:

  ConnectionPainter();

public:

  static
  QPainterPath
  cubicPath(ConnectionGeometry const& geom);

  static
  QPainterPath
  getPainterStroke(ConnectionGeometry const& geom);

  static
  void
  paint(QPainter* painter,
        std::shared_ptr<Connection> const &connection);
};
