#pragma once

#include "PortType.hpp"

#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include <iostream>

namespace QtNodes
{

/**
 * @brief The ConnectionGeometry class holds the aspects of a connection's
 * graphical object geometry in the FlowScene. Each connection is associated
 * with a unique geometry object.
 */
class ConnectionGeometry
{
public:

  ConnectionGeometry();

public:

  QPointF const&
  getEndPoint(PortType portType) const;

  void
  setEndPoint(PortType portType, QPointF const& point);

  void
  moveEndPoint(PortType portType, QPointF const &offset);

  QRectF
  boundingRect() const;

  std::pair<QPointF, QPointF>
  pointsC1C2() const;

  QPointF
  source() const
  {
    return _out;
  }
  QPointF
  sink() const
  {
    return _in;
  }

  double
  lineWidth() const
  {
    return _lineWidth;
  }

  bool
  hovered() const
  {
    return _hovered;
  }

  bool
  frozen() const
  {
    return _frozen;
  }

  void
  setHovered(bool hovered)
  {
    _hovered = hovered;
  }

  void
  setFrozen(bool frozen)
  {
    _frozen = frozen;
  }

private:
  // local object coordinates
  QPointF _in;
  QPointF _out;

  //int _animationPhase;

  double _lineWidth;

  bool _hovered;

  bool _frozen;

};
}
