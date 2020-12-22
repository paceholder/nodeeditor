#pragma once

#include <QtGui/QPainter>

namespace QtNodes
{

class Node;
class NodeState;
class NodeGeometry;
class NodeGraphicsObject;
class NodeDataModel;
class NodeStyle;
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

  /**
   * @brief Draws the icon indicating the node's current processing status.
   * @param painter Painter to be used
   * @param geom Node geometry
   * @param model Node model
   */
  static
  void
  drawStatusIcon(QPainter * painter,
                 NodeGeometry const & geom);

  /**
   * @brief Draws the progress value percentage in the processing node.
   * @param painter Painter to be used
   * @param geom Node geometry
   * @param model Node model
   */
  static
  void
  drawProgressValue(QPainter * painter,
                    NodeGeometry const & geom,
                    NodeStyle const & nodeStyle,
                    QString const & nodeProgress);

};
}
