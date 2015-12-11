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
    , _animationPhase(0)
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
    double protectOvershooting = 200;

    QPointF addon(_pointDiameter + protectOvershooting,
                  _pointDiameter + protectOvershooting);

    QPointF minimum(qMin(_source.x(), _sink.x()),
                    qMin(_source.y(), _sink.y()));

    QPointF maximum(qMax(_source.x(), _sink.x()),
                    qMax(_source.y(), _sink.y()));

    return QRectF(minimum - addon, maximum + addon);
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

  int _animationPhase;

  double _lineWidth;

  bool _hovered;
};

#endif //  CONNECTION_GEOMETRY_H
