#pragma once

#include <memory>

#include <QtGui/QPainter>

class NodeGraphicsObject;
class NodeGeometry;
class NodeState;
class Node;
class NodeDataModel;
class FlowItemEntry;

class NodePainter
{
public:

  NodePainter();

public:

  static
  void
  paint(QPainter* painter,
        Node& node);

  static
  void
  drawNodeRect(QPainter* painter, NodeGeometry const& geom,
               NodeGraphicsObject const & graphicsObject);

  static
  void
  drawModelName(QPainter* painter,
                NodeGeometry const& geom,
                NodeState const& state,
                NodeDataModel* const model);

  static
  void
  drawEntryLabels(QPainter* painter,
                  NodeGeometry const& geom,
                  NodeState const& state,
                  NodeDataModel* const model);

  static
  void
  drawConnectionPoints(QPainter* painter,
                       NodeGeometry const& geom,
                       NodeState const& state,
                       NodeDataModel* const model);

  static
  void
  drawFilledConnectionPoints(QPainter* painter,
                             NodeGeometry const& geom,
                             NodeState const& state,
                             NodeDataModel* const model);

  static
  void
  drawResizeRect(QPainter* painter,
                 NodeGeometry const& geom,
                 NodeDataModel* const model);
  
  static
  void
  drawErrorRect(QPainter * painter,
                NodeGeometry const & geom,
                NodeDataModel* const model);
};
