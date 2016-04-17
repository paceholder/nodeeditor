#include "ConnectionGeometry.hpp"

ConnectionGeometry::
ConnectionGeometry()
  : _source(10, 10)
  , _sink(100, 100)
  , _pointDiameter(10)
  //, _animationPhase(0)
  , _lineWidth(3.0)
  , _hovered(false)
{ }

QPointF const&
ConnectionGeometry::
getEndPoint(EndType endType) const
{
  Q_ASSERT(endType != EndType::NONE);

  return (endType == EndType::SOURCE ?
          _source :
          _sink);
}


void
ConnectionGeometry::
setEndPoint(EndType endType, QPointF const& point)
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


void
ConnectionGeometry::
moveEndPoint(EndType endType, QPointF const &offset)
{
  switch (endType)
  {
    case EndType::SOURCE:
      _source += offset;
      break;

    case EndType::SINK:
      _sink += offset;
      break;

    default:
      break;
  }
}


QRectF
ConnectionGeometry::
boundingRect() const
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
ConnectionGeometry::
pointsC1C2() const
{
  double xDistance = _sink.x() - _source.x();
  //double yDistance = _sink.y() - _source.y() - 100;

  double defaultOffset = 200;

  double minimum = qMin(defaultOffset, std::abs(xDistance));

  double verticalOffset = 0;

  double ratio1 = 0.5;

  if (xDistance <= 0)
  {
    verticalOffset = -minimum;

    ratio1 = 1.0;
  }

  //double verticalOffset2 = verticalOffset;
  //if (xDistance <= 0)
  //verticalOffset2 = qMin(defaultOffset, std::abs(yDistance));
  //auto sign = [](double d) { return d > 0.0 ? +1.0 : -1.0; };
  //verticalOffset2 = 0.0;

  QPointF c1(_source.x() + minimum * ratio1,
             _source.y() + verticalOffset);

  QPointF c2(_sink.x() - minimum * ratio1,
             _sink.y() + verticalOffset);

  return std::make_pair(c1, c2);
}
