#include "NodePainter.hpp"

#include <QtCore/QMargins>

#include "NodeGeometry.hpp"
#include "NodeState.hpp"
#include "NodeDataModel.hpp"
#include "Node.hpp"

void
NodePainter::
paint(QPainter* painter,
      std::shared_ptr<Node> const &node)
{
  NodeGeometry const& geom = node->nodeGeometry();

  NodeState const& state = node->nodeState();

  geom.recalculateSize(painter->fontMetrics());

  //--------------------------------------------

  drawNodeRect(painter, geom);

  drawConnectionPoints(painter, geom, state);

  drawFilledConnectionPoints(painter, geom, state);

  auto const &model = node->nodeDataModel();

  drawEntryLabels(painter, geom, state, model);
}


void
NodePainter::
drawNodeRect(QPainter* painter,
             NodeGeometry const& geom)
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

  QLinearGradient gradient(QPointF(0.0, 0.0),
                           QPointF(2.0, geom.height()));

  QColor darkGray1 = QColor(Qt::gray).darker(200);
  QColor darkGray2 = QColor(Qt::gray).darker(250);

  gradient.setColorAt(0.0,  Qt::darkGray);
  gradient.setColorAt(0.03, darkGray1);
  gradient.setColorAt(0.97, darkGray2);
  gradient.setColorAt(1.0,  darkGray2.darker(110));

  painter->setBrush(gradient);

  unsigned int diam = geom.connectionPointDiameter();
  QRectF   boundary(0.0, 0.0, geom.width(), geom.height());
  QMargins m(diam, diam, diam, diam);

  double const radius = 3.0;

  painter->drawRoundedRect(boundary.marginsAdded(m), radius, radius);
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
    [&](PortType portType)
    {
      size_t n = state.getEntries(portType).size();

      for (size_t i = 0; i < n; ++i)
      {

        QPointF p = geom.portScenePosition(i, portType);

        double r = 1.0;
        if (state.isReacting() &&
            !state.getEntries(portType)[i].lock())
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

  drawPoints(PortType::OUT);
  drawPoints(PortType::IN);
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
    [&](PortType portType)
    {
      size_t n = state.getEntries(portType).size();

      for (size_t i = 0; i < n; ++i)
      {
        QPointF p = geom.portScenePosition(i, portType);

        if (state.getEntries(portType)[i].lock())
        {
          painter->drawEllipse(p,
                               diameter * 0.4,
                               diameter * 0.4);
        }
      }
    };

  drawPoints(PortType::OUT);
  drawPoints(PortType::IN);
}


void
NodePainter::
drawEntryLabels(QPainter* painter,
                NodeGeometry const& geom,
                NodeState const& state,
                std::unique_ptr<NodeDataModel> const & model)
{
  QFontMetrics const & metrics =
    painter->fontMetrics();

  auto drawPoints =
    [&](PortType portType)
    {
      auto& entries = state.getEntries(portType);

      size_t n = entries.size();

      for (size_t i = 0; i < n; ++i)
      {

        QPointF p = geom.portScenePosition(i, portType);

        if (entries[i].lock())
          painter->setPen(QColor(Qt::lightGray).lighter());
        else
          painter->setPen(Qt::darkGray);

        QString s = model->data(portType, i)->name();

        auto rect = metrics.boundingRect(s);

        p.setY(p.y() + rect.height() / 4.0);

        if (portType == PortType::IN)
          p.setX(5.0);
        else
          p.setX(geom.width() - 5.0 - rect.width());

        painter->drawText(p, s);
      }
    };

  drawPoints(PortType::OUT);
  drawPoints(PortType::IN);
}
