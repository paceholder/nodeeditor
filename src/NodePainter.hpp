#ifndef NODE_PAINTER_HPP
#define NODE_PAINTER_HPP

#include <QtGui/QPainter>

#include "EndType.hpp"

class NodeGeometry;
class NodeState;
class FlowItemEntry;

class NodePainter
{
public:

  NodePainter();

public:

  static
  void paint(QPainter* painter,
             NodeGeometry const& geom,
             NodeState const& state);

  static
  void drawNodeRect(QPainter* painter, NodeGeometry const& geom);

  static
  void drawEntryLabels(QPainter* painter,
                       NodeGeometry const& geom,
                       NodeState const& state);

  static
  void drawConnectionPoints(QPainter* painter,
                            NodeGeometry const& geom,
                            NodeState const& state);

  static
  void drawFilledConnectionPoints(QPainter* painter,
                                  NodeGeometry const& geom,
                                  NodeState const& state);
};

#endif //  NODE_PAINTER_HPP
