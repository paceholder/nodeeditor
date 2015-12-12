#ifndef CONNECTION_PAINTER_HPP
#define CONNECTION_PAINTER_HPP

#include <QtGui/QPainter>


class ConnectionGeometry;

class ConnectionPainter
{
public:

  ConnectionPainter(ConnectionGeometry& connectionGeometry);

public:

  QPainterPath cubicPath() const;

  QPainterPath getPainterStroke() const;

  void paint(QPainter* painter) const;

private:
  ConnectionGeometry& _connectionGeometry;
};

#endif //  CONNECTION_PAINTER_HPP
