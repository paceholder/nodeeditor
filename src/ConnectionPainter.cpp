#include "ConnectionPainter.hpp"

//#define DEBUG_DRAWING 1

#include "ConnectionGeometry.hpp"

ConnectionPainter::
ConnectionPainter(ConnectionGeometry& connectionGeometry)
  : _connectionGeometry(connectionGeometry)
{}

QPainterPath
ConnectionPainter::
cubicPath() const
{
  QPointF const& source = _connectionGeometry.source();
  QPointF const& sink   = _connectionGeometry.sink();

  auto c1c2 = _connectionGeometry.pointsC1C2();

  // cubic spline
  QPainterPath cubic(source);

  cubic.cubicTo(c1c2.first, c1c2.second, sink);

  return cubic;
}


QPainterPath
ConnectionPainter::
getPainterStroke() const
{
  auto cubic = cubicPath();

  QPointF const& source = _connectionGeometry.source();
  QPainterPath result(source);

  unsigned segments = 20;

  for (auto i = 0ul; i < segments; ++i)
  {
    double ratio = double(i + 1) / segments;
    result.lineTo(cubic.pointAtPercent(ratio));
  }

  QPainterPathStroker stroker; stroker.setWidth(10.0);

  return stroker.createStroke(result);
}


void
ConnectionPainter::
paint(QPainter* painter) const
{
  bool const hovered = _connectionGeometry.hovered();

  double const lineWidth     = _connectionGeometry.lineWidth();
  double const pointDiameter = _connectionGeometry.pointDiameter();

#ifdef DEBUG_DRAWING

  {
    QPointF const& source = _connectionGeometry.source();
    QPointF const& sink   = _connectionGeometry.sink();

    auto points = _connectionGeometry.pointsC1C2();

    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);

    painter->drawLine(QLineF(source, points.first));
    painter->drawLine(QLineF(points.first, points.second));
    painter->drawLine(QLineF(points.second, sink));
    painter->drawEllipse(points.first, 4, 4);
    painter->drawEllipse(points.second, 4, 4);

    painter->setBrush(Qt::NoBrush);

    painter->drawPath(cubicPath());
  }
#endif

  auto cubic = cubicPath();

  if (hovered)
  {
    QPen p;

    p.setWidth(2 * lineWidth);
    p.setColor(QColor(Qt::cyan).lighter());
    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);

    // cubic spline

    painter->drawPath(cubic);
  }

  QPen p;

  p.setWidth(lineWidth);
  p.setColor(QColor(Qt::cyan).darker());
  painter->setPen(p);
  painter->setBrush(Qt::NoBrush);

  // cubic spline
  painter->drawPath(cubic);

  QPointF const& source = _connectionGeometry.source();
  QPointF const& sink   = _connectionGeometry.sink();

  painter->setPen(Qt::white);
  painter->setBrush(Qt::white);
  double const pointRadius = pointDiameter / 2.0;
  painter->drawEllipse(source, pointRadius, pointRadius);
  painter->drawEllipse(sink, pointRadius, pointRadius);
}
