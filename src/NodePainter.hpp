#pragma once

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
class ConnectionStyle;

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
               Node const& node,
               NodeGraphicsObject const & graphicsObject);

  static
  void
  drawModelName(QPainter* painter,
                NodeGeometry const& geom,
                NodeState const& state,
                Node const & node);

  static
  void
  drawEntryLabels(QPainter* painter,
                  NodeGeometry const& geom,
                  NodeState const& state,
                  Node const & node);

  static
  void
  drawConnectionPoints(QPainter* painter,
                       NodeGeometry const& geom,
                       NodeState const& state,
                       Node const & node,
                       FlowScene const & scene,
                       ConnectionStyle const & connectionStyle);

  static
  void
  drawFilledConnectionPoints(QPainter* painter,
                             NodeGeometry const& geom,
                             NodeState const& state,
                             Node const & node,
                             ConnectionStyle const & connectionStyle);

  static
  void
  drawResizeRect(QPainter* painter,
                 NodeGeometry const& geom,
                 Node const & node);

  static
  void
  drawValidationRect(QPainter * painter,
                     NodeGeometry const & geom,
                     Node const & node,
                     NodeGraphicsObject const & graphicsObject);
};
}
