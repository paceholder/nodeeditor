#ifndef CONNECTION_GEOMETRY_H
#define CONNECTION_GEOMETRY_H

#include "EndType.hpp"

#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include <iostream>

class ConnectionGeometry
{
public:

  ConnectionGeometry()
    : _source(10, 10)
    , _sink(100, 100)
    , _pointDiameter(10)
    //, _animationPhase(0)
    , _lineWidth(3.0)
    , _hovered(false)
  { }

public:

  inline
  QPointF& getEndPoint(EndType endType)
  {
    Q_ASSERT(endType != EndType::NONE);

    return (endType == EndType::SOURCE ?
            _source :
            _sink);
  }

  void setEndPoint(EndType endType, QPointF const& point)
  {
    switch (endType)
    {
      case EndType::SOURCE:
        _source = point;
        break;

      case EndType::SINK:
        _sink = point;
        break;

      default:
        break;
    }
  }

  inline
  QRectF boundingRect() const
  {
    auto points = pointsC1C2();

    QRectF basicRect(_source, _sink);

    QRectF c1c2Rect(points.first, points.second);

    QMargins margins(_pointDiameter,
                     _pointDiameter,
                     _pointDiameter,
                     _pointDiameter);

    return basicRect.united(c1c2Rect).marginsAdded(margins);
  }

  std::pair<QPointF, QPointF>
  pointsC1C2() const
  {
    double xDistance = _sink.x() - _source.x();
    double const defaultOffset = 200;

    double minimum = qMin(defaultOffset, std::abs(xDistance));

    double verticalOffset = 0;

    double ratio1 = 0.5;

    if (xDistance <= 0)
    {
      verticalOffset = -minimum;

      ratio1 = 1.0;
    }

    QPointF c1(_source.x() + minimum * ratio1,
               _source.y() + 2 * verticalOffset);

    ratio1 = 0.5;

    QPointF c2(_sink.x() - minimum * ratio1,
               _sink.y() + verticalOffset);

    return std::make_pair(c1, c2);
  }

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
