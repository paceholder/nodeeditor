#pragma once

#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtGui/QTransform>
#include <QtGui/QFontMetrics>

#include "Export.hpp"
#include "Definitions.hpp"

namespace QtNodes
{

class AbstractGraphModel;
class NodeGraphicsObject;

/**
 * A helper-class for manipulating the node's geometry.
 * It is designed to be constructed on stack and used in-place.
 * The class is in essense a wrapper around the AbstractGraphModel.
 */
class NODE_EDITOR_PUBLIC NodeGeometry
{
public:
  NodeGeometry(NodeGraphicsObject const & ngo);

public:

  unsigned int entryHeight() const;

  unsigned int verticalSpacing() const;

public:
  QRectF boundingRect() const;

  QSize size() const;

  /// Updates size unconditionally
  QSize recalculateSize() const;

  /// Updates size if the QFontMetrics is changed
  QSize recalculateSizeIfFontChanged(QFont const & font) const;

  QPointF portNodePosition(PortType const portType,
                           PortIndex const index) const;

  QPointF portScenePosition(PortType const portType,
                            PortIndex const index,
                            QTransform const & t) const;

  PortIndex checkHitScenePoint(PortType portType,
                               QPointF point,
                               QTransform const & t = QTransform()) const;

  QRect resizeRect() const;

  /// Returns the position of a widget on the Node surface
  QPointF widgetPosition() const;

  /// Returns the maximum height a widget can be without causing the node to grow.
  int equivalentWidgetHeight() const;

  //unsigned int validationHeight() const;

  //unsigned int validationWidth() const;

  //static
  //QPointF calculateNodePositionBetweenNodePorts(PortIndex targetPortIndex, PortType targetPort, Node * targetNode,
  //PortIndex sourcePortIndex, PortType sourcePort, Node * sourceNode,
  //Node & newNode);
private:

  unsigned int captionHeight() const;

  unsigned int captionWidth() const;

  unsigned int portWidth(PortType portType) const;

private:

  NodeGraphicsObject const & _ngo;
  AbstractGraphModel & _graphModel;

  // some variables are mutable because
  // we need to change drawing metrics
  // corresponding to fontMetrics
  // but this doesn't change constness of Node

  mutable unsigned int _defaultInPortWidth;
  mutable unsigned int _defaultOutPortWidth;
  mutable unsigned int _entryHeight;
  unsigned int _verticalSpacing;
  mutable QFontMetrics _fontMetrics;
  mutable QFontMetrics _boldFontMetrics;

  //unsigned int _entryWidth; // TODO: where was is used?

  //unsigned int _nSources; // TODO: where was it used?
  //unsigned int _nSinks;

  //QPointF _draggingPos;

  //std::unique_ptr<NodeDataModel> const & _dataModel;

};
}
