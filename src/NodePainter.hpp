#pragma once

#include <memory>

#include <QtGui/QPainter>

namespace QtNodes
{

class Node;
class NodeState;
class NodeGeometry;
class NodeGraphicsObject;
class NodeDataModel;
class FlowItemEntry;
class FlowScene;

class NodePainter
{
public:

  NodePainter();

public:

  static
  void
  paint(QPainter* painter,
        Node& node,
        FlowScene const& scene);

  static
  void
  drawNodeRect(QPainter* painter,
               NodeGeometry const& geom,
               NodeDataModel const* model,
               NodeGraphicsObject const & graphicsObject);

  static
  void
  drawModelName(QPainter* painter,
                NodeGeometry const& geom,
                NodeState const& state,
                NodeDataModel const * model);

  static
  void
  drawEntryLabels(QPainter* painter,
                  NodeGeometry const& geom,
                  NodeState const& state,
                  NodeDataModel const * model);

  static
  void
  drawConnectionPoints(QPainter* painter,
                       NodeGeometry const& geom,
                       NodeState const& state,
                       NodeDataModel const * model,
                       FlowScene const & scene);

  static
  void
  drawFilledConnectionPoints(QPainter* painter,
                             NodeGeometry const& geom,
                             NodeState const& state,
                             NodeDataModel const * model);

  static
  void
  drawResizeRect(QPainter* painter,
                 NodeGeometry const& geom,
                 NodeDataModel const * model);

  static
  void
  drawValidationRect(QPainter * painter,
                     NodeGeometry const & geom,
                     NodeDataModel const * model,
                     NodeGraphicsObject const & graphicsObject);
};
}
