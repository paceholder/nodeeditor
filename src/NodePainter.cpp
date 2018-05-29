#include "NodePainter.hpp"

#include <cmath>

#include <QtCore/QMargins>

#include "ConnectionStyle.hpp"
#include "PortType.hpp"
#include "NodeGraphicsObject.hpp"
#include "NodeGeometry.hpp"
#include "NodeState.hpp"
#include "FlowScene.hpp"
#include "NodeDataModel.hpp"
#include "Node.hpp"
#include "NodeStyle.hpp"

using QtNodes::ConnectionStyle;
using QtNodes::NodePainter;
using QtNodes::NodeGeometry;
using QtNodes::NodeGraphicsObject;
using QtNodes::Node;
using QtNodes::NodeState;
using QtNodes::NodeStyle;
using QtNodes::NodeDataModel;
using QtNodes::FlowScene;

void
NodePainter::
paint(QPainter* painter,
      Node & node,
      FlowScene const& scene)
{
  NodeGeometry const& geom = node.nodeGeometry();

  NodeState const& state = node.nodeState();

  NodeGraphicsObject const & graphicsObject = node.nodeGraphicsObject();

  geom.recalculateSize(painter->font());

  //--------------------------------------------
  NodeDataModel const * model = node.nodeDataModel();

  drawNodeRect(painter, geom, node, graphicsObject);

  drawConnectionPoints(painter, geom, state, node, scene, scene.connectionStyle());

  drawFilledConnectionPoints(painter, geom, state, node, scene.connectionStyle());

  drawModelName(painter, geom, state, node);

  drawEntryLabels(painter, geom, state, node);

  drawResizeRect(painter, geom, node);

  drawValidationRect(painter, geom, node, graphicsObject);

  /// call custom painter
  if (auto painterDelegate = model->painterDelegate())
  {
    painterDelegate->paint(painter, geom, model);
  }
}


void
NodePainter::
drawNodeRect(QPainter* painter,
             NodeGeometry const& geom,
             Node const& node,
             NodeGraphicsObject const & graphicsObject)
{
  NodeStyle const& nodeStyle = node.nodeStyle();

  auto color = graphicsObject.isSelected()
               ? nodeStyle.selectedBoundaryColor()
               : nodeStyle.normalBoundaryColor();

  if (geom.hovered())
  {
    QPen p(color, nodeStyle.hoveredPenWidth());
    painter->setPen(p);
  }
  else
  {
    QPen p(color, nodeStyle.penWidth());
    painter->setPen(p);
  }

  QLinearGradient gradient(QPointF(0.0, 0.0),
                           QPointF(2.0, geom.height()));

  gradient.setColorAt(0.0, nodeStyle.gradientColor0());
  gradient.setColorAt(0.03, nodeStyle.gradientColor1());
  gradient.setColorAt(0.97, nodeStyle.gradientColor2());
  gradient.setColorAt(1.0, nodeStyle.gradientColor3());

  painter->setBrush(gradient);

  float diam = nodeStyle.connectionPointDiameter();

  QRectF boundary( -diam, -diam, 2.0 * diam + geom.width(), 2.0 * diam + geom.height());

  double const radius = 3.0;

  painter->drawRoundedRect(boundary, radius, radius);
}


void
NodePainter::
drawConnectionPoints(QPainter* painter,
                     NodeGeometry const & geom,
                     NodeState const & state,
                     Node const & node,
                     FlowScene const & scene,
                     ConnectionStyle const &connectionStyle)
{
  NodeDataModel const &model = *node.nodeDataModel();

  NodeStyle const &nodeStyle = node.nodeStyle();

  float diameter = nodeStyle.connectionPointDiameter();
  auto  reducedDiameter = diameter * 0.6;

  auto drawPoints =
    [&](PortType portType)
    {
      size_t n = state.getEntries(portType).size();

      for (unsigned int i = 0; i < n; ++i)
      {
        QPointF p = geom.portScenePosition(i, portType);

        auto const & dataType = model.dataType(portType, i);

        bool canConnect = (state.getEntries(portType)[i].empty() ||
                           (portType == PortType::Out &&
                            model.portOutConnectionPolicy(i) == NodeDataModel::ConnectionPolicy::Many) );

        double r = 1.0;
        if (state.isReacting() &&
            canConnect &&
            portType == state.reactingPortType())
        {

          auto   diff = geom.draggingPos() - p;
          double dist = std::sqrt(QPointF::dotProduct(diff, diff));
          bool   typeConvertable = false;

          {
            if (portType == PortType::In)
            {
              typeConvertable = scene.registry().getTypeConverter(state.reactingDataType(), dataType) != nullptr;
            }
            else
            {
              typeConvertable = scene.registry().getTypeConverter(dataType, state.reactingDataType()) != nullptr;
            }
          }

          if (state.reactingDataType().id == dataType.id || typeConvertable)
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
          painter->setBrush(ConnectionStyle::computeNormalColor(dataType.id));
        }
        else
        {
          painter->setBrush(nodeStyle.connectionPointColor());
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
                           Node const & node,
                           ConnectionStyle const & connectionStyle)
{
  NodeDataModel const &model = *node.nodeDataModel();

  NodeStyle const &nodeStyle = node.nodeStyle();

  auto diameter = nodeStyle.connectionPointDiameter();

  auto drawPoints =
    [&](PortType portType)
    {
      size_t n = state.getEntries(portType).size();

      for (size_t i = 0; i < n; ++i)
      {
        QPointF p = geom.portScenePosition(i, portType);

        if (!state.getEntries(portType)[i].empty())
        {
          auto const & dataType = model.dataType(portType, i);

          if (connectionStyle.useDataDefinedColors())
          {
            QColor const c = ConnectionStyle::computeNormalColor(dataType.id);
            painter->setPen(c);
            painter->setBrush(c);
          }
          else
          {
            painter->setPen(nodeStyle.filledConnectionPointColor());
            painter->setBrush(nodeStyle.filledConnectionPointColor());
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
              Node const &node)
{
  NodeDataModel const &model = *node.nodeDataModel();

  NodeStyle const& nodeStyle = node.nodeStyle();

  Q_UNUSED(state);

  if (!model.captionVisible())
    return;

  QString const &name = model.caption();

  QFont f = painter->font();

  f.setBold(true);

  QFontMetrics metrics(f);

  auto rect = metrics.boundingRect(name);

  QPointF position((geom.width() - rect.width()) / 2.0,
                   (geom.spacing() + geom.entryHeight()) / 3.0);

  painter->setFont(f);
  painter->setPen(nodeStyle.fontColor());
  painter->drawText(position, name);

  f.setBold(false);
  painter->setFont(f);
}


void
NodePainter::
drawEntryLabels(QPainter * painter,
                NodeGeometry const & geom,
                NodeState const & state,
                Node const &node)
{
  NodeDataModel const &model = *node.nodeDataModel();

  QFontMetrics const & metrics =
    painter->fontMetrics();

  auto drawPoints =
    [&](PortType portType)
    {
      auto const &nodeStyle = node.nodeStyle();

      auto& entries = state.getEntries(portType);

      size_t n = entries.size();

      for (size_t i = 0; i < n; ++i)
      {
        QPointF p = geom.portScenePosition(i, portType);

        if (entries[i].empty())
          painter->setPen(nodeStyle.fontColorFaded());
        else
          painter->setPen(nodeStyle.fontColor());

        QString s;

        if (model.portCaptionVisible(portType, i))
        {
          s = model.portCaption(portType, i);
        }
        else
        {
          s = model.dataType(portType, i).name;
        }

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
               Node const &node)
{
  NodeDataModel const &model = *node.nodeDataModel();

  if (model.resizable())
  {
    painter->setBrush(Qt::gray);

    painter->drawEllipse(geom.resizeRect());
  }
}


void
NodePainter::
drawValidationRect(QPainter * painter,
                   NodeGeometry const & geom,
                   Node const & node,
                   NodeGraphicsObject const & graphicsObject)
{
  NodeDataModel const &model = *node.nodeDataModel();

  auto modelValidationState = model.validationState();

  if (modelValidationState != NodeValidationState::Valid)
  {
    NodeStyle const& nodeStyle = node.nodeStyle();

    auto color = graphicsObject.isSelected()
                 ? nodeStyle.selectedBoundaryColor()
                 : nodeStyle.normalBoundaryColor();

    if (geom.hovered())
    {
      QPen p(color, nodeStyle.hoveredPenWidth());
      painter->setPen(p);
    }
    else
    {
      QPen p(color, nodeStyle.penWidth());
      painter->setPen(p);
    }

    //Drawing the validation message background
    if (modelValidationState == NodeValidationState::Error)
    {
      painter->setBrush(nodeStyle.errorColor());
    }
    else
    {
      painter->setBrush(nodeStyle.warningColor());
    }

    double const radius = 3.0;

    float diam = nodeStyle.connectionPointDiameter();

    QRectF boundary(-diam,
                    -diam + geom.height() - geom.validationHeight(),
                    2.0 * diam + geom.width(),
                    2.0 * diam + geom.validationHeight());

    painter->drawRoundedRect(boundary, radius, radius);

    painter->setBrush(Qt::gray);

    //Drawing the validation message itself
    QString const &errorMsg = model.validationMessage();

    QFont f = painter->font();

    QFontMetrics metrics(f);

    auto rect = metrics.boundingRect(errorMsg);

    QPointF position((geom.width() - rect.width()) / 2.0,
                     geom.height() - (geom.validationHeight() - diam) / 2.0);

    painter->setFont(f);
    painter->setPen(nodeStyle.fontColor());
    painter->drawText(position, errorMsg);
  }
}
