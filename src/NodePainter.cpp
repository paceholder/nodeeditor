#include "NodePainter.hpp"

#include <cmath>

#include <QtCore/QMargins>

#include "StyleCollection.hpp"
#include "PortType.hpp"
#include "NodeGraphicsObject.hpp"
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

  NodeGraphicsObject const & graphicsObject = node->nodeGraphicsObject();

  geom.recalculateSize(painter->font());

  //--------------------------------------------

  drawNodeRect(painter, geom, graphicsObject);

  auto const &model = node->nodeDataModel();

  drawConnectionPoints(painter, geom, state, model);

  drawFilledConnectionPoints(painter, geom, state, model);

  drawModelName(painter, geom, state, model);

  drawEntryLabels(painter, geom, state, model);

  drawResizeRect(painter, geom, model);
}


void
NodePainter::
drawNodeRect(QPainter* painter,
             NodeGeometry const& geom,
             NodeGraphicsObject const & graphicsObject)
{
  NodeStyle const& nodeStyle = StyleCollection::nodeStyle();

  auto color = graphicsObject.isSelected()
               ? nodeStyle.SelectedBoundaryColor
               : nodeStyle.NormalBoundaryColor;

  if (geom.hovered())
  {
    QPen p(color, nodeStyle.HoveredPenWidth);
    painter->setPen(p);
  }
  else
  {
    QPen p(color, nodeStyle.PenWidth);
    painter->setPen(p);
  }

  QLinearGradient gradient(QPointF(0.0, 0.0),
                           QPointF(2.0, geom.height()));

  gradient.setColorAt(0.0,  nodeStyle.GradientColor0);
  gradient.setColorAt(0.03, nodeStyle.GradientColor1);
  gradient.setColorAt(0.97, nodeStyle.GradientColor2);
  gradient.setColorAt(1.0,  nodeStyle.GradientColor3);

  painter->setBrush(gradient);

  float diam = nodeStyle.ConnectionPointDiameter;

  QRectF    boundary(0.0, 0.0, geom.width(), geom.height());
  QMarginsF m(diam, diam, diam, diam);

  double const radius = 3.0;

  painter->drawRoundedRect(boundary.marginsAdded(m), radius, radius);
}


void
NodePainter::
drawConnectionPoints(QPainter* painter,
                     NodeGeometry const& geom,
                     NodeState const& state,
                     NodeDataModel* const model)
{
  NodeStyle const& nodeStyle      = StyleCollection::nodeStyle();
  auto const     &connectionStyle = StyleCollection::connectionStyle();

  float diameter = nodeStyle.ConnectionPointDiameter;
  auto  reducedDiameter = diameter * 0.6;

  auto drawPoints =
  [&](PortType portType)
  {
    size_t n = state.getEntries(portType).size();

    for (size_t i = 0; i < n; ++i)
    {

      QPointF p = geom.portScenePosition(i, portType);

      auto const & dataType = model->dataType(portType, i);

      double r = 1.0;
      if (state.isReacting() &&
          (state.getEntries(portType)[i].empty() ||
           portType == PortType::Out) &&
          portType == state.reactingPortType())
      {

        auto   diff = geom.draggingPos() - p;
        double dist = std::sqrt(QPointF::dotProduct(diff, diff));

        if (state.reactingDataType().id == dataType.id)
        {
          double const thres = 40.0;
          r = (dist < thres) ?
              (2.0 - dist / thres ) :
              1.0;
        }
        else
        {
          double const thres = 80.0;
          r = (dist < thres) ?
              (dist / thres) :
              1.0;
        }
      }

      if (connectionStyle.useDataDefinedColors())
      {
        painter->setBrush(connectionStyle.normalColor(dataType.id));
      }
      else
      {
        painter->setBrush(nodeStyle.ConnectionPointColor);
      }

      painter->drawEllipse(p,
                           reducedDiameter * r,
                           reducedDiameter * r);
    }
  };

  drawPoints(PortType::Out);
  drawPoints(PortType::In);
}


void
NodePainter::
drawFilledConnectionPoints(QPainter * painter,
                           NodeGeometry const & geom,
                           NodeState const & state,
                           NodeDataModel* const model)
{
  NodeStyle const& nodeStyle       = StyleCollection::nodeStyle();
  auto const     & connectionStyle = StyleCollection::connectionStyle();

  auto diameter = nodeStyle.ConnectionPointDiameter;

  auto drawPoints =
  [&](PortType portType)
  {
    size_t n = state.getEntries(portType).size();

    for (size_t i = 0; i < n; ++i)
    {
      QPointF p = geom.portScenePosition(i, portType);

      if (!state.getEntries(portType)[i].empty())
      {
        auto const & dataType = model->dataType(portType, i);

        if (connectionStyle.useDataDefinedColors())
        {
          QColor const c = connectionStyle.normalColor(dataType.id);
          painter->setPen(c);
          painter->setBrush(c);
        }
        else
        {
          painter->setPen(nodeStyle.FilledConnectionPointColor);
          painter->setBrush(nodeStyle.FilledConnectionPointColor);
        }

        painter->drawEllipse(p,
                             diameter * 0.4,
                             diameter * 0.4);
      }
    }
  };

  drawPoints(PortType::Out);
  drawPoints(PortType::In);
}


void
NodePainter::
drawModelName(QPainter * painter,
              NodeGeometry const & geom,
              NodeState const & state,
              NodeDataModel* const model)
{
  NodeStyle const& nodeStyle = StyleCollection::nodeStyle();

  Q_UNUSED(state);

  if (!model->captionVisible())
    return;

  QString const &name = model->caption();

  QFont f = painter->font();

  f.setBold(true);

  QFontMetrics metrics(f);

  auto rect = metrics.boundingRect(name);

  QPointF position((geom.width() - rect.width()) / 2.0,
                   (geom.spacing() + geom.entryHeight()) / 3.0);

  painter->setFont(f);
  painter->setPen(nodeStyle.FontColor);
  painter->drawText(position, name);

  f.setBold(false);
  painter->setFont(f);
}


void
NodePainter::
drawEntryLabels(QPainter * painter,
                NodeGeometry const & geom,
                NodeState const & state,
                NodeDataModel* const model)
{
  QFontMetrics const & metrics =
    painter->fontMetrics();

  auto drawPoints =
  [&](PortType portType)
  {
    auto const &nodeStyle = StyleCollection::nodeStyle();

    auto& entries = state.getEntries(portType);

    size_t n = entries.size();

    for (size_t i = 0; i < n; ++i)
    {
      QPointF p = geom.portScenePosition(i, portType);

      if (entries[i].empty())
        painter->setPen(nodeStyle.FontColorFaded);
      else
        painter->setPen(nodeStyle.FontColor);

      QString s = model->dataType(portType, i).name;

      auto rect = metrics.boundingRect(s);

      p.setY(p.y() + rect.height() / 4.0);

      switch (portType)
      {
        case PortType::In:
          p.setX(5.0);
          break;

        case PortType::Out:
          p.setX(geom.width() - 5.0 - rect.width());
          break;

        default:
          break;
      }

      painter->drawText(p, s);
    }
  };

  drawPoints(PortType::Out);

  drawPoints(PortType::In);
}


void
NodePainter::
drawResizeRect(QPainter * painter,
               NodeGeometry const & geom,
               NodeDataModel* const model)
{
  if (model->resizable())
  {
    painter->setBrush(Qt::gray);

    painter->drawEllipse(geom.resizeRect());
  }
}
