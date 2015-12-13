#ifndef NODE_PAINTER_HPP
#define NODE_PAINTER_HPP

#include <QtGui/QPainter>

#include "EndType.hpp"

class NodeGeometry;
class FlowItemEntry;

class NodePainter
{
public:

  NodePainter();

public:

  static
  void paint(QPainter* painter,
             NodeGeometry const& geom,
             std::vector<FlowItemEntry*>const & sources,
             std::vector<FlowItemEntry*>const & sinks);

  static
  void drawConnectionPoints(QPainter* painter,
                            NodeGeometry const& geom);

  static
  void drawFilledConnectionPoints(QPainter* painter,
                                  NodeGeometry const& geom,
                                  std::vector<FlowItemEntry*>const & sources,
                                  std::vector<FlowItemEntry*>const & sinks);
};

#endif //  NODE_PAINTER_HPP
