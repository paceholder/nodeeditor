#ifndef CONNECTION_PAINTER_HPP
#define CONNECTION_PAINTER_HPP

#include <QtGui/QPainter>

#include "ConnectionGeometry.hpp"

//#define DEBUG_DRAWING 1

class ConnectionPainter
{
public:
  ConnectionPainter(ConnectionGeometry& connectionGeometry)
    : _connectionGeometry(connectionGeometry)
  {}

public:

  QPainterPath painterPath() const
  {
    QPainterPath path;

    QPointF const& source = _connectionGeometry.source();
    QPointF const& sink   = _connectionGeometry.sink();

    QPointF diff = (sink - source);

    double xDistance = sink.x() - source.x();
    double distance  = std::sqrt(QPointF::dotProduct(diff, diff));

    double const defaultOffset = 200;

    double const lineWidth     = _connectionGeometry.lineWidth();

    double const ratio1 = 0.5;

    double verticalOffset = 0;

    if (xDistance <= 0)
    {
      verticalOffset = -qMin(defaultOffset, std::abs(xDistance));
    }

    QPointF c1(source.x() + defaultOffset * ratio1, source.y() + 2 * verticalOffset);
    QPointF c2(sink.x() - defaultOffset * ratio1, sink.y() + verticalOffset);

    QPen p;

    p.setWidth(2 * lineWidth);

    // cubic spline
    QPainterPath cubic(source);
    cubic.cubicTo(c1, c2, sink);

    path.addPath(cubic);

    return path;
  }

  inline
  void paint(QPainter* painter) const
  {
    QPointF const& source = _connectionGeometry.source();
    QPointF const& sink   = _connectionGeometry.sink();

    bool const hovered = _connectionGeometry.hovered();

    QPointF diff = (sink - source);

    double xDistance = sink.x() - source.x();
    double distance  = std::sqrt(QPointF::dotProduct(diff, diff));

    double const defaultOffset = 200;

    double const lineWidth     = _connectionGeometry.lineWidth();
    double const pointDiameter = _connectionGeometry.pointDiameter();

    double const ratio1 = 0.5;
    //double const ratio2 = 1 - ratio1;
    //QPointF c1(sink.x() * ratio2 + source.x() * ratio1, source.y());
    //QPointF c2(sink.x() * ratio1 + source.x() * ratio2, sink.y());

    double verticalOffset = 0;

    if (xDistance <= 0)
    {
      verticalOffset = -qMin(defaultOffset, std::abs(xDistance));
    }

    QPointF c1(source.x() + defaultOffset * ratio1, source.y() + 2 * verticalOffset);
    QPointF c2(sink.x() - defaultOffset * ratio1, sink.y() + verticalOffset);

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

    if (hovered)
    {
      QPen p;

      p.setWidth(2 * lineWidth);
      p.setColor(QColor(Qt::cyan).lighter());
      painter->setPen(p);
      painter->setBrush(Qt::NoBrush);

      // cubic spline
      QPainterPath path(source);
      path.cubicTo(c1, c2, sink);
      painter->drawPath(path);
    }

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
  ConnectionGeometry& _connectionGeometry;
};

#endif //  CONNECTION_PAINTER_HPP
