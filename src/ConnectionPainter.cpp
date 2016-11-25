#include "ConnectionPainter.hpp"

//#define DEBUG_DRAWING 1

#include "ConnectionGeometry.hpp"
#include "ConnectionState.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "Connection.hpp"

ConnectionStyle ConnectionPainter::connectionStyle;

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
      std::shared_ptr<Connection> const &connection)
{
  ConnectionGeometry const& geom =
    connection->connectionGeometry();

  ConnectionState const& state =
    connection->connectionState();

  double const lineWidth     = connectionStyle.LineWidth;
  double const pointDiameter = connectionStyle.PointDiameter;

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

  auto const& graphicsObject =
    connection->getConnectionGraphicsObject();

  bool const selected = graphicsObject->isSelected();

  if (hovered || selected)
  {
    QPen p;

    p.setWidth(2 * lineWidth);
    p.setColor(selected ?
               connectionStyle.SelectedHaloColor :
               connectionStyle.HoveredColor);

    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);

    // cubic spline

    painter->drawPath(cubic);
  }

  // draw normal line
  {
    QPen p;

    p.setWidth(lineWidth);

    if (selected)
      p.setColor(connectionStyle.SelectedColor);
    else
      p.setColor(connectionStyle.NormalColor);

    if (state.requiresPort())
    {
      p.setWidth(connectionStyle.ConstructionLineWidth);
      p.setColor(connectionStyle.ConstructionColor);
      p.setStyle(Qt::DashLine);
    }

    painter->setPen(p);
    painter->setBrush(Qt::NoBrush);

    // cubic spline
    painter->drawPath(cubic);
  }

  QPointF const& source = geom.source();
  QPointF const& sink   = geom.sink();

  painter->setPen(connectionStyle.ConstructionColor);
  painter->setBrush(connectionStyle.ConstructionColor);
  double const pointRadius = pointDiameter / 2.0;
  painter->drawEllipse(source, pointRadius, pointRadius);
  painter->drawEllipse(sink, pointRadius, pointRadius);
}
