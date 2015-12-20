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
                           QPointF(10.0, geom.height()));

  QColor darkGray1 = QColor(Qt::gray).darker(200);
  QColor darkGray2 = QColor(Qt::gray).darker(250);

  gradient.setColorAt(0.0,  darkGray1);
  gradient.setColorAt(0.95,  darkGray2);
  gradient.setColorAt(1.0,  darkGray2.darker(110));

  painter->setBrush(gradient);

  unsigned int diam = geom.connectionPointDiameter();
  QRectF   boundary(0.0, 0.0, geom.width(), geom.height());
  QMargins m(diam, diam, diam, diam);

  double const radius = 3.0;

  painter->drawRoundedRect(boundary.marginsAdded(m), radius, radius);

  drawConnectionPoints(painter, geom);

  drawFilledConnectionPoints(painter, geom, state);
}


void
NodePainter::
drawConnectionPoints(QPainter* painter,
                     NodeGeometry const& geom)
{
  painter->setBrush(QColor(Qt::darkGray));
  double totalHeight = 0;

  auto diameter = geom.connectionPointDiameter();

  double const h = geom.entryHeight();
  for (size_t i = 0; i < geom.nSinks(); ++i)
  {
    double y = totalHeight + (geom.spacing()  + h) / 2;
    double x = 0.0 - diameter;

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
    double x = geom.width() + diameter;

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

    totalHeight += h + geom.spacing();
  }
}


void
NodePainter::
drawFilledConnectionPoints(QPainter* painter,
                           NodeGeometry const& geom,
                           NodeState const& state)
{
  painter->setPen(Qt::cyan);
  painter->setBrush(Qt::cyan);

  double totalHeight = 0;

  auto diameter = geom.connectionPointDiameter();

  double const h = geom.entryHeight();

  for (size_t i = 0; i < geom.nSinks(); ++i)
  {

    double y = totalHeight + (geom.spacing()  + h) / 2;
    double x = 0.0 - diameter;

    if (!state.connectionID(EndType::SINK, i).isNull())
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
    double x = geom.width() + diameter;

    if (!state.connectionID(EndType::SOURCE, i).isNull())
    {
      painter->drawEllipse(QPointF(x, y),
                           diameter * 0.4,
                           diameter * 0.4);
    }

    totalHeight += h + geom.spacing();
  }
}
