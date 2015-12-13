#include "NodePainter.hpp"

#include "NodeGeometry.hpp"

#include "FlowItemEntry.hpp"

#include <QtCore/QMargins>

void
NodePainter::
paint(QPainter* painter,
      NodeGeometry const& geom,
      std::vector<FlowItemEntry*>const & sources,
      std::vector<FlowItemEntry*>const & sinks)
{

  if (geom.hovered())
  {
    QPen p(Qt::white, 2.0);
    painter->setPen(p);
  }
  else
  {
    QPen p(Qt::white, 1.5);
    painter->setPen(p);
  }

  painter->setBrush(QColor(Qt::darkGray));

  unsigned int diam = geom.connectionPointDiameter();
  QRectF   boundary(0.0, 0.0, geom.width(), geom.height());
  QMargins m(diam, diam, diam, diam);

  double const radius = 3.0;

  painter->drawRoundedRect(boundary.marginsAdded(m), radius, radius);

  drawConnectionPoints(painter, geom);

  drawFilledConnectionPoints(painter, geom, sources, sinks);
}


void
NodePainter::
drawConnectionPoints(QPainter* painter,
                     NodeGeometry const& geom)
{
  painter->setBrush(QColor(Qt::darkGray));
  double totalHeight = 0;

  auto diameter = geom.connectionPointDiameter();

  // TODO: make class-member
  double const offsetMult = 1.3;

  double const h = geom.entryHeight();
  for (size_t i = 0; i < geom.nSinks(); ++i)
  {
    double y = totalHeight + (geom.spacing()  + h) / 2;
    double x = 0.0 - diameter * offsetMult;

    QPointF p(x, y);

    auto   diff = geom.draggingPos() - p;
    double dist = std::sqrt(QPointF::dotProduct(diff, diff));

    double const thres = 40.0;

    double const r = (dist < thres) ?
                     (2.0 - dist / thres / 2.) :
                     1.0;

    painter->drawEllipse(p,
                         diameter * 0.6 * r,
                         diameter * 0.6 * r);

    totalHeight += h + geom.spacing();
  }

  totalHeight += geom.spacing();

  for (size_t i = 0; i < geom.nSources(); ++i)
  {
    double y = totalHeight + (geom.spacing()  + h) / 2;
    double x = geom.width() + diameter * offsetMult;

    QPointF p(x, y);

    auto   diff = geom.draggingPos() - p;
    double dist = std::sqrt(QPointF::dotProduct(diff, diff));

    double const thres = 20.0;

    double const r = (dist < thres) ?
                     (2.0 - dist / thres / 2.) :
                     1.0;

    painter->drawEllipse(p,
                         diameter * 0.6 * r,
                         diameter * 0.6 * r);
    painter->drawEllipse(p,
                         diameter * 0.6,
                         diameter * 0.6);

    totalHeight += h + geom.spacing();
  }
}


void
NodePainter::
drawFilledConnectionPoints(QPainter* painter,
                           NodeGeometry const& geom,
                           std::vector<FlowItemEntry*> const& sources,
                           std::vector<FlowItemEntry*> const& sinks)
{
  painter->setPen(Qt::cyan);
  painter->setBrush(Qt::cyan);

  double totalHeight = 0;

  auto diameter = geom.connectionPointDiameter();

  double const h = geom.entryHeight();

  for (size_t i = 0; i < geom.nSinks(); ++i)
  {

    double y = totalHeight + (geom.spacing()  + h) / 2;
    double x = 0.0 - diameter * 1.3;

    if (!sinks[i]->getConnectionID().isNull())
    {

      painter->drawEllipse(QPointF(x, y),
                           diameter * 0.4,
                           diameter * 0.4);
    }

    totalHeight += h + geom.spacing();
  }

  totalHeight += geom.spacing();

  for (size_t i = 0; i < geom.nSources(); ++i)
  {
    double y = totalHeight + (geom.spacing()  + h) / 2;
    double x = geom.width() + diameter * 1.3;

    if (!sources[i]->getConnectionID().isNull())
    {
      painter->drawEllipse(QPointF(x, y),
                           diameter * 0.4,
                           diameter * 0.4);
    }

    totalHeight += h + geom.spacing();
  }
}
