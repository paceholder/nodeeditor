#ifndef CONNECTION_GEOMETRY_H
#define CONNECTION_GEOMETRY_H

#include "EndType.hpp"

#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include <iostream>

class ConnectionGeometry
{
public:

  ConnectionGeometry();

public:

  QPointF& getEndPoint(EndType endType);

  void setEndPoint(EndType endType, QPointF const& point);

  QRectF boundingRect() const;

  std::pair<QPointF, QPointF>
  pointsC1C2() const;

  QPointF source() const { return _source; }
  QPointF sink() const { return _sink; }

  double pointDiameter() const { return _pointDiameter; }

  double lineWidth() const { return _lineWidth; }

  bool hovered() const { return _hovered; }
  void setHovered(bool hovered) { _hovered = hovered; }

private:
  // local object coordinates
  QPointF _source;
  QPointF _sink;

  double _pointDiameter;

  //int _animationPhase;

  double _lineWidth;

  bool _hovered;
};

#endif //  CONNECTION_GEOMETRY_H
