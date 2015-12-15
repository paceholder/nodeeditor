#ifndef NODE_GEOMETRY_HPP
#define NODE_GEOMETRY_HPP

#include <QtCore/QRectF>
#include <QtCore/QPointF>

#include "EndType.hpp"

class NodeGeometry
{
public:

  NodeGeometry();

public:
  unsigned int height() const { return _height; }
  void setHeight(unsigned int h) { _height = h; }

  unsigned int width() const { return _width; }
  void setWidth(unsigned int w) { _width = w; }

  unsigned int entryHeight() const { return _entryHeight; }
  void setEntryHeight(unsigned int h) { _entryHeight = h; }

  unsigned int entryWidth() const { return _entryWidth; }
  void setEntryWidth(unsigned int w) { _entryWidth = w; }

  unsigned int spacing() const { return _spacing; }
  void setSpacing(unsigned int s) { _spacing = s; }

  unsigned int connectionPointDiameter() const
  { return _connectionPointDiameter; }
  void setconnectionPointDiameter(unsigned int d)
  { _connectionPointDiameter = d; }

  bool hovered() const { return _hovered; }
  void setHovered(unsigned int h) { _hovered = h; }

  unsigned int nSources() const { return _nSources; }
  void setNSources(unsigned int nSources)
  { _nSources = nSources; }

  unsigned int nSinks() const { return _nSinks; }
  void setNSinks(unsigned int nSinks)
  { _nSinks = nSinks; }

  QPointF const& draggingPos() const
  { return _draggingPos; }
  void setDraggingPosition(QPointF const& pos)
  { _draggingPos = pos; }

  double opacity() const { return _opacity; }

public:
  QRectF entryBoundingRect() const;

  QRectF boundingRect() const;

  void recalculateSize();

  QPointF connectionPointScenePosition(int index,
                                       EndType endType) const;

private:
  unsigned int _width;
  unsigned int _height;
  unsigned int _entryWidth;
  unsigned int _entryHeight;
  unsigned int _spacing;

  unsigned int _connectionPointDiameter;

  bool _hovered;

  unsigned int _nSources;
  unsigned int _nSinks;

  QPointF _draggingPos;

  double _opacity;
};

#endif //  NODE_GEOMETRY_HPP
