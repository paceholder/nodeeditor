#include "NodePainter.hpp"

#include <cmath>

#include <QtCore/QMargins>

#include "AbstractGraphModel.hpp"
#include "NodeGeometry.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeState.hpp"
#include "PortType.hpp"
#include "StyleCollection.hpp"


namespace QtNodes
{

void
NodePainter::
paint(QPainter * painter,
      NodeGraphicsObject const &ngo)
{
  NodeGeometry geometry(ngo);
  geometry.recalculateSizeIfFontChanged(painter->font());

  drawNodeRect(painter, ngo);

  drawConnectionPoints(painter, ngo);

  drawFilledConnectionPoints(painter, ngo);

  drawNodeCaption(painter, ngo);

  drawEntryLabels(painter, ngo);

  drawResizeRect(painter, ngo);
}


void
NodePainter::
drawNodeRect(QPainter * painter,
             NodeGraphicsObject const &ngo)
{
  AbstractGraphModel const &model = ngo.graphModel();

  NodeId const nodeId = ngo.nodeId();

  NodeGeometry geom(ngo);
  QSize size = geom.size();

  QJsonDocument json =
    QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));

  NodeStyle nodeStyle(json);

  auto color = ngo.isSelected() ?
               nodeStyle.SelectedBoundaryColor :
               nodeStyle.NormalBoundaryColor;

  if (ngo.nodeState().hovered())
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
                           QPointF(2.0, size.height()));

  gradient.setColorAt(0.0,  nodeStyle.GradientColor0);
  gradient.setColorAt(0.03, nodeStyle.GradientColor1);
  gradient.setColorAt(0.97, nodeStyle.GradientColor2);
  gradient.setColorAt(1.0,  nodeStyle.GradientColor3);

  painter->setBrush(gradient);

  float diam = nodeStyle.ConnectionPointDiameter;

  QRectF boundary(-diam, -diam,
                  2.0 * diam + size.width(),
                  2.0 * diam + size.height());

  double const radius = 3.0;

  painter->drawRoundedRect(boundary, radius, radius);
}


void
NodePainter::
drawConnectionPoints(QPainter * painter,
                     NodeGraphicsObject const &ngo)
{
  AbstractGraphModel const &model = ngo.graphModel();
  NodeId const nodeId     = ngo.nodeId();
  NodeGeometry geom(ngo);

  QJsonDocument json =
    QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
  NodeStyle nodeStyle(json);

  auto const &connectionStyle = StyleCollection::connectionStyle();

  float diameter       = nodeStyle.ConnectionPointDiameter;
  auto reducedDiameter = diameter * 0.6;

  for (PortType portType: {PortType::Out, PortType::In})
  {
    size_t const n =
      model.nodeData(nodeId,
                     (portType == PortType::Out) ?
                     NodeRole::NumberOfOutPorts :
                     NodeRole::NumberOfInPorts).toUInt();

    for (PortIndex portIndex = 0; portIndex < n; ++portIndex)
    {
      QPointF p = geom.portNodePosition(portType, portIndex);

      auto const &dataType =
        model.portData(nodeId,
                       portType,
                       portIndex,
                       PortRole::DataType).value<NodeDataType>();

      auto const &connectedNodes =
        model.connectedNodes(nodeId, portType, portIndex);

      bool canConnect =
        (connectedNodes.empty() ||
         model.portData(nodeId,
                        portType,
                        portIndex,
                        PortRole::ConnectionPolicyRole).value<ConnectionPolicy>() == ConnectionPolicy::Many);

      double r = 1.0;

      NodeState const &state = ngo.nodeState();

      if (state.isReacting() &&
          canConnect &&
          portType == state.reactingPortType())
      {
        auto diff   = state.draggingPos() - p;
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
  }
  ;
}


void
NodePainter::
drawFilledConnectionPoints(QPainter * painter,
                           NodeGraphicsObject const &ngo)
{
  AbstractGraphModel const &model = ngo.graphModel();
  NodeId const nodeId     = ngo.nodeId();
  NodeGeometry geom(ngo);

  QJsonDocument json =
    QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
  NodeStyle nodeStyle(json);

  auto diameter = nodeStyle.ConnectionPointDiameter;

  for (PortType portType: {PortType::Out, PortType::In})
  {
    size_t const n =
      model.nodeData(nodeId,
                     (portType == PortType::Out) ?
                     NodeRole::NumberOfOutPorts :
                     NodeRole::NumberOfInPorts).toUInt();

    for (PortIndex portIndex = 0; portIndex < n; ++portIndex)
    {
      QPointF p = geom.portNodePosition(portType, portIndex);

      auto const &connectedNodes =
        model.connectedNodes(nodeId, portType, portIndex);

      if (!connectedNodes.empty())
      {
        auto const &dataType =
          model.portData(nodeId,
                         portType,
                         portIndex,
                         PortRole::DataType).value<NodeDataType>();

        auto const &connectionStyle = StyleCollection::connectionStyle();
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
  }
}


void
NodePainter::
drawNodeCaption(QPainter * painter,
                NodeGraphicsObject const &ngo)
{
  AbstractGraphModel const &model = ngo.graphModel();
  NodeId const nodeId     = ngo.nodeId();
  NodeGeometry geom(ngo);

  if (!model.nodeData(nodeId, NodeRole::CaptionVisible).toBool())
    return;

  QString const name = model.nodeData(nodeId, NodeRole::Caption).toString();

  QFont f = painter->font();
  f.setBold(true);

  QFontMetrics metrics(f);
  auto rect  = metrics.boundingRect(name);
  QSize size = geom.size();

  QPointF position((size.width() - rect.width()) / 2.0,
                   (geom.verticalSpacing() + geom.entryHeight()) / 3.0);

  QJsonDocument json =
    QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
  NodeStyle nodeStyle(json);

  painter->setFont(f);
  painter->setPen(nodeStyle.FontColor);
  painter->drawText(position, name);

  f.setBold(false);
  painter->setFont(f);
}


void
NodePainter::
drawEntryLabels(QPainter * painter,
                NodeGraphicsObject const &ngo)
{
  AbstractGraphModel const &model = ngo.graphModel();
  NodeId const nodeId     = ngo.nodeId();
  NodeGeometry geom(ngo);

  QJsonDocument json =
    QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
  NodeStyle nodeStyle(json);

  QSize size = geom.size();

  for (PortType portType: {PortType::Out, PortType::In})
  {
    size_t const n =
      model.nodeData(nodeId,
                     (portType == PortType::Out) ?
                     NodeRole::NumberOfOutPorts :
                     NodeRole::NumberOfInPorts).toUInt();

    for (PortIndex portIndex = 0; portIndex < n; ++portIndex)
    {
      auto const &connectedNodes =
        model.connectedNodes(nodeId, portType, portIndex);

      QPointF p = geom.portNodePosition(portType, portIndex);

      if (connectedNodes.empty())
        painter->setPen(nodeStyle.FontColorFaded);
      else
        painter->setPen(nodeStyle.FontColor);

      QString s;

      if (model.portData(nodeId, portType, portIndex, PortRole::CaptionVisible).toBool())
      {
        s = model.portData(nodeId, portType, portIndex, PortRole::Caption).toString();
      }
      else
      {
        auto portData =
          model.portData(nodeId, portType, portIndex, PortRole::DataType);

        s = portData.value<NodeDataType>().name;
      }

      QFontMetrics const &metrics = painter->fontMetrics();
      auto rect = metrics.boundingRect(s);

      p.setY(p.y() + rect.height() / 4.0);

      switch (portType)
      {
        case PortType::In:
          p.setX(5.0);
          break;

        case PortType::Out:
          p.setX(size.width() - 5.0 - rect.width());
          break;

        default:
          break;
      }

      painter->drawText(p, s);
    }
  }
}


void
NodePainter::
drawResizeRect(QPainter * painter,
               NodeGraphicsObject const &ngo)
{
  AbstractGraphModel const &model = ngo.graphModel();
  NodeId const nodeId     = ngo.nodeId();
  NodeGeometry geom(ngo);

  if (model.nodeFlags(nodeId) & NodeFlag::Resizable)
  {
    painter->setBrush(Qt::gray);

    painter->drawEllipse(geom.resizeRect());
  }
}


}
