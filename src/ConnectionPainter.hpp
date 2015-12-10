#ifndef CONNECTION_PAINTER_HPP
#define CONNECTION_PAINTER_HPP

#include <QtGui/QPainter>

#include "ConnectionGeometry.hpp"

class ConnectionPainter
{
public:
  ConnectionPainter(ConnectionGeometry const& connectionGeometry)
    : _connectionGeometry(connectionGeometry)
  {}

public:
  void paint(QPainter* painter) const
  {
    QPointF const& source = _connectionGeometry.source();
    QPointF const& sink = _connectionGeometry.sink();
    double const lineWidth = _connectionGeometry.lineWidth();
    double const pointDiameter = _connectionGeometry.pointDiameter();

    double const ratio1 = 0.5;
    double const ratio2 = 1 - ratio1;
    QPointF c1(sink.x() * ratio2 + source.x() * ratio1, source.y());
    QPointF c2(sink.x() * ratio1 + source.x() * ratio2, sink.y());

#ifdef DEBUG_DRAWING

    {
      painter->setPen(Qt::red);
      painter->setBrush(Qt::red);

      painter->drawLine(QLineF(source, c1));
      painter->drawLine(QLineF(c1, c2));
      painter->drawLine(QLineF(c2, sink));
      painter->drawEllipse(c1, 4, 4);
      painter->drawEllipse(c2, 4, 4);
    }
#endif

    QPen p;
    p.setWidth(lineWidth);
    p.setColor(QColor(Qt::cyan).darker());
    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);

    // cubic spline
    QPainterPath path(source);
    path.cubicTo(c1, c2, sink);
    painter->drawPath(path);

    //path.angleAtPercent(t)

    painter->setPen(Qt::white);
    painter->setBrush(Qt::white);

    double const pointRadius = pointDiameter / 2.0;
    painter->drawEllipse(source, pointRadius, pointRadius);
    painter->drawEllipse(sink, pointRadius, pointRadius);
  }

private:
  ConnectionGeometry const& _connectionGeometry;
};

#endif //  CONNECTION_PAINTER_HPP
