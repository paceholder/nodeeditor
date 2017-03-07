#include "ConnectionGeometry.hpp"

#include <cmath>

#include "StyleCollection.hpp"

using QtNodes::ConnectionGeometry;
using QtNodes::PortType;

ConnectionGeometry::
ConnectionGeometry()
  : _in(0, 0)
  , _out(0, 0)
  //, _animationPhase(0)
  , _lineWidth(3.0)
  , _hovered(false)
{ }

QPointF const&
ConnectionGeometry::
getEndPoint(PortType portType) const
{
  Q_ASSERT(portType != PortType::None);

  return (portType == PortType::Out ?
          _out :
          _in);
}


void
ConnectionGeometry::
setEndPoint(PortType portType, QPointF const& point)
{
  switch (portType)
  {
    case PortType::Out:
      _out = point;
      break;

    case PortType::In:
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
    case PortType::Out:
      _out += offset;
      break;

    case PortType::In:
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

  auto const &connectionStyle =
    StyleCollection::connectionStyle();

  float const diam = connectionStyle.pointDiameter();

  c1c2Rect.setTop(  -diam + c1c2Rect.top());
  c1c2Rect.setLeft( -diam + c1c2Rect.left());
  c1c2Rect.setBottom( 2.0*diam + c1c2Rect.bottom());
  c1c2Rect.setRight(  2.0*diam + c1c2Rect.right());

  return basicRect.united(c1c2Rect);
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
