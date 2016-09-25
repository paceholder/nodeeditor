#pragma once

#include <memory>

#include <QtGui/QPainter>

#include "PortType.hpp"

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
        std::shared_ptr<Node> const &node);

  static
  void
  drawNodeRect(QPainter* painter, NodeGeometry const& geom,
               std::unique_ptr<NodeGraphicsObject> const& graphicsObject);

  static
  void
  drawModelName(QPainter* painter,
                NodeGeometry const& geom,
                NodeState const& state,
                std::unique_ptr<NodeDataModel> const & model);

  static
  void
  drawEntryLabels(QPainter* painter,
                  NodeGeometry const& geom,
                  NodeState const& state,
                  std::unique_ptr<NodeDataModel> const & model);

  static
  void
  drawConnectionPoints(QPainter* painter,
                       NodeGeometry const& geom,
                       NodeState const& state,
                       std::unique_ptr<NodeDataModel> const & model);

  static
  void
  drawFilledConnectionPoints(QPainter* painter,
                             NodeGeometry const& geom,
                             NodeState const& state);

  static
  void
  drawResizeRect(QPainter* painter,
                 NodeGeometry const& geom,
                 std::unique_ptr<NodeDataModel> const & model);
};
