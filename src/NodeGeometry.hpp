#pragma once

#include <memory>

#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtGui/QTransform>
#include <QtGui/QFontMetrics>

#include "PortType.hpp"

class NodeState;
class NodeDataModel;

class NodeGeometry
{
public:

  NodeGeometry(std::unique_ptr<NodeDataModel> const &dataModel);

public:
  unsigned int
  height() const { return _height; }

  void
  setHeight(unsigned int h) { _height = h; }

  unsigned int
  width() const { return _width; }

  void
  setWidth(unsigned int w) { _width = w; }

  unsigned int
  entryHeight() const { return _entryHeight; }
  void
  setEntryHeight(unsigned int h) { _entryHeight = h; }

  unsigned int
  entryWidth() const { return _entryWidth; }

  void
  setEntryWidth(unsigned int w) { _entryWidth = w; }

  unsigned int
  spacing() const { return _spacing; }

  void
  setSpacing(unsigned int s) { _spacing = s; }

  unsigned int
  connectionPointDiameter() const
  { return _connectionPointDiameter; }

  void
  setconnectionPointDiameter(unsigned int d)
  { _connectionPointDiameter = d; }

  bool
  hovered() const { return _hovered; }

  void
  setHovered(unsigned int h) { _hovered = h; }

  unsigned int
  nSources() const { return _nSources; }

  unsigned int
  nSinks() const { return _nSinks; }

  QPointF const&
  draggingPos() const
  { return _draggingPos; }

  void
  setDraggingPosition(QPointF const& pos)
  { _draggingPos = pos; }

  double
  opacity() const { return _opacity; }

public:

  QRectF
  entryBoundingRect() const;

  QRectF
  boundingRect() const;

  /// Updates size unconditionally
  void
  recalculateSize() const;

  /// Updates size if the QFontMetrics is changed
  void
  recalculateSize(QFontMetrics const &fontMetrics) const;

  // TODO removed default QTransform()
  QPointF
  portScenePosition(PortIndex index,
                    PortType portType,
                    QTransform t = QTransform()) const;

  PortIndex
  checkHitScenePoint(PortType portType,
                     QPointF const point,
                     QTransform t = QTransform()) const;

  QRect
  resizeRect() const;

  /// Returns the position of a widget on the Node surface
  QPointF
  widgetPosition() const;

private:

  unsigned int
  nameHeight() const;

  unsigned int
  portWidth(PortType portType) const;

private:

  // some variables are mutable because
  // we need to change drawing metrics
  // corresponding to fontMetrics
  // but this doesn't change constness of Node

  mutable unsigned int _width;
  mutable unsigned int _height;
  unsigned int _entryWidth;
  mutable unsigned int _inputPortWidth;
  mutable unsigned int _outputPortWidth;
  mutable unsigned int _entryHeight;
  unsigned int _spacing;

  unsigned int _connectionPointDiameter;

  bool _hovered;

  unsigned int _nSources;
  unsigned int _nSinks;

  QPointF _draggingPos;

  double _opacity;

  std::unique_ptr<NodeDataModel> const &_dataModel;

  mutable QFontMetrics _fontMetrics;
};
