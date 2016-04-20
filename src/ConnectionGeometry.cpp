#include "ConnectionGeometry.hpp"

ConnectionGeometry::
ConnectionGeometry()
  : _out(10, 10)
  , _in(100, 100)
  , _pointDiameter(10)
  //, _animationPhase(0)
  , _lineWidth(3.0)
  , _hovered(false)
{ }

QPointF const&
ConnectionGeometry::
getEndPoint(PortType portType) const
{
  Q_ASSERT(portType != PortType::NONE);

  return (portType == PortType::OUT ?
          _out :
          _in);
}


void
ConnectionGeometry::
setEndPoint(PortType portType, QPointF const& point)
{
  switch (portType)
  {
    case PortType::OUT:
      _out = point;
      break;

    case PortType::IN:
      _in = point;
      break;

    default:
      break;
  }
}


void
ConnectionGeometry::
moveEndPoint(PortType portType, QPointF const &offset)
{
  switch (portType)
  {
    case PortType::OUT:
      _out += offset;
      break;

    case PortType::IN:
      _in += offset;
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

  QRectF basicRect(_out, _in);

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
  double xDistance = _in.x() - _out.x();
  //double yDistance = _in.y() - _out.y() - 100;

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

  QPointF c1(_out.x() + minimum * ratio1,
             _out.y() + verticalOffset);

  QPointF c2(_in.x() - minimum * ratio1,
             _in.y() + verticalOffset);

  return std::make_pair(c1, c2);
}
