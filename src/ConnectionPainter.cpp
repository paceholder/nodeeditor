#include "ConnectionPainter.hpp"

//#define DEBUG_DRAWING 1

#include "ConnectionGeometry.hpp"
#include "ConnectionState.hpp"

ConnectionPainter::
ConnectionPainter()
{}

QPainterPath
ConnectionPainter::
cubicPath(ConnectionGeometry const& geom)
{
  QPointF const& source = geom.source();
  QPointF const& sink   = geom.sink();

  auto c1c2 = geom.pointsC1C2();

  // cubic spline
  QPainterPath cubic(source);

  cubic.cubicTo(c1c2.first, c1c2.second, sink);

  return cubic;
}


QPainterPath
ConnectionPainter::
getPainterStroke(ConnectionGeometry const& geom)
{
  auto cubic = cubicPath(geom);

  QPointF const& source = geom.source();
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
paint(QPainter* painter,
      ConnectionGeometry const& geom,
      ConnectionState const& state)
{
  double const lineWidth     = geom.lineWidth();
  double const pointDiameter = geom.pointDiameter();

#ifdef DEBUG_DRAWING

  {
    QPointF const& source = geom.source();
    QPointF const& sink   = geom.sink();

    auto points = geom.pointsC1C2();

    painter->setPen(Qt::red);
    painter->setBrush(Qt::red);

    painter->drawLine(QLineF(source, points.first));
    painter->drawLine(QLineF(points.first, points.second));
    painter->drawLine(QLineF(points.second, sink));
    painter->drawEllipse(points.first, 4, 4);
    painter->drawEllipse(points.second, 4, 4);

    painter->setBrush(Qt::NoBrush);

    painter->drawPath(cubicPath(geom));
  }
#endif

  auto cubic = cubicPath(geom);

  bool const hovered = geom.hovered();
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

  // draw normal line
  {
    QPen p;

    p.setWidth(lineWidth);
    p.setColor(QColor(Qt::cyan).darker(150));

    if (state.requiresPort())
    {
      p.setWidth(2.0);
      p.setColor(QColor(Qt::gray));
      p.setStyle(Qt::DashLine);
    }

    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);

    // cubic spline
    painter->drawPath(cubic);
  }

  QPointF const& source = geom.source();
  QPointF const& sink   = geom.sink();

  painter->setPen(Qt::white);
  painter->setBrush(Qt::white);
  double const pointRadius = pointDiameter / 2.0;
  painter->drawEllipse(source, pointRadius, pointRadius);
  painter->drawEllipse(sink, pointRadius, pointRadius);
}
