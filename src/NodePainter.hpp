#pragma once

#include <QtGui/QPainter>

#include "Definitions.hpp"

namespace QtNodes
{

class BasicGraphicsScene;
class GraphModel;
class NodeGeometry;
class NodeGraphicsObject;
class NodeState;

/// @ Lightweight class incapsulating paint code.
class NodePainter
{
public:

  NodePainter();

public:

  static
  void paint(QPainter * painter,
             NodeGraphicsObject const & ngo);

  static
  void drawNodeRect(QPainter * painter,
                    NodeGraphicsObject const & ngo);

  static
  void drawConnectionPoints(QPainter * painter,
                            NodeGraphicsObject const & ngo);
  static
  void drawFilledConnectionPoints(QPainter * painter,
                                  NodeGraphicsObject const & ngo);

  static
  void drawNodeCaption(QPainter * painter,
                       NodeGraphicsObject const & ngo);

  static
  void drawEntryLabels(QPainter * painter,
                       NodeGraphicsObject const & ngo);

  static
  void drawResizeRect(QPainter * painter,
                      NodeGraphicsObject const & ngo);
};
}
