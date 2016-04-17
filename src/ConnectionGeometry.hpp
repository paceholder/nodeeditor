#pragma once

#include "EndType.hpp"

#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include <iostream>

class ConnectionGeometry
{
public:

  ConnectionGeometry();

public:

  QPointF const& getEndPoint(EndType endType) const;

  void setEndPoint(EndType endType, QPointF const& point);

  void moveEndPoint(EndType endType, QPointF const &offset);

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
