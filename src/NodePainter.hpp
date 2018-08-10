#pragma once

#include <QtGui/QPainter>

namespace QtNodes
{

class NodeState;
class NodeGeometry;
class NodeGraphicsObject;
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
        NodeGraphicsObject const & graphicsObject);

  static
  void
  drawNodeRect(QPainter* painter,
               NodeGraphicsObject const & graphicsObject);

  static
  void
  drawModelName(QPainter* painter,
                NodeGraphicsObject const & graphicsObject);

  static
  void
  drawEntryLabels(QPainter* painter,
                  NodeGraphicsObject const & graphicsObject);

  static
  void
  drawConnectionPoints(QPainter* painter,
                       NodeGraphicsObject const & graphicsObject);

  static
  void
  drawFilledConnectionPoints(QPainter* painter,
                             NodeGraphicsObject const & graphicsObject);

  static
  void
  drawResizeRect(QPainter* painter,
                 NodeGraphicsObject const & graphicsObject);

  static
  void
  drawValidationRect(QPainter * painter,
                     NodeGraphicsObject const & graphicsObject);
};
}
