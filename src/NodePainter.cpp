#include "NodePainter.hpp"

#include <QtCore/QMargins>

#include "NodeGeometry.hpp"
#include "NodeState.hpp"

void
NodePainter::
paint(QPainter* painter,
      NodeGeometry const& geom,
      NodeState const& state)
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

  //painter->setBrush(QColor(Qt::darkGray));

  QLinearGradient gradient(QPointF(0.0, 0.0),
                           QPointF(5.0, geom.height()));

  QColor darkGray1 = QColor(Qt::gray).darker(200);
  QColor darkGray2 = QColor(Qt::gray).darker(250);

  gradient.setColorAt(0.0,  Qt::gray);
  gradient.setColorAt(0.05, darkGray1);
  gradient.setColorAt(0.95, darkGray2);
  gradient.setColorAt(1.0,  darkGray2.darker(110));

  painter->setBrush(gradient);

  unsigned int diam = geom.connectionPointDiameter();
  QRectF   boundary(0.0, 0.0, geom.width(), geom.height());
  QMargins m(diam, diam, diam, diam);

  double const radius = 3.0;

  painter->drawRoundedRect(boundary.marginsAdded(m), radius, radius);

  drawConnectionPoints(painter, geom, state);

  drawFilledConnectionPoints(painter, geom, state);
}


void
NodePainter::
drawConnectionPoints(QPainter* painter,
                     NodeGeometry const& geom,
                     NodeState const& state)
{
  painter->setBrush(QColor(Qt::darkGray));

  auto diameter = geom.connectionPointDiameter();
  auto reducedDiameter = diameter * 0.6;

  auto drawPoints =
    [&](EndType end)
    {
      size_t n = state.getEntries(end).size();

      for (size_t i = 0; i < n; ++i)
      {

        QPointF p = geom.connectionPointScenePosition(i, end);

        double r = 1.0;
        if (state.isReacting())
        {
          auto   diff = geom.draggingPos() - p;
          double dist = std::sqrt(QPointF::dotProduct(diff, diff));

          double const thres = 40.0;

          r = (dist < thres) ?
              (2.0 - dist / thres ) :
              1.0;
        }

        painter->drawEllipse(p,
                             reducedDiameter * r,
                             reducedDiameter * r);
      }
    };

  drawPoints(EndType::SOURCE);
  drawPoints(EndType::SINK);
}


void
NodePainter::
drawFilledConnectionPoints(QPainter* painter,
                           NodeGeometry const& geom,
                           NodeState const& state)
{
  painter->setPen(Qt::cyan);
  painter->setBrush(Qt::cyan);

  auto diameter = geom.connectionPointDiameter();

  auto drawPoints =
    [&](EndType end)
    {
      size_t n = state.getEntries(end).size();

      for (size_t i = 0; i < n; ++i)
      {
        QPointF p = geom.connectionPointScenePosition(i, end);

        if (!state.connectionID(end, i).isNull())
        {
          painter->drawEllipse(p,
                               diameter * 0.4,
                               diameter * 0.4);
        }
      }
    };

  drawPoints(EndType::SOURCE);
  drawPoints(EndType::SINK);
}
