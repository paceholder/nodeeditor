#include "NodeGeometry.hpp"

NodeGeometry::
NodeGeometry()
  : _width(100)
  , _height(150)
  , _entryWidth(100)
  , _entryHeight(20)
  , _spacing(10)
  , _connectionPointDiameter(10)
  , _hovered(false)
  , _draggingPos(-1000, -1000)
{
  //
}


QRectF
NodeGeometry::
entryBoundingRect() const
{
  double addon = 0;

  return QRectF(0 - addon,
                0 - addon,
                _entryWidth + 2 * addon,
                _entryHeight + 2 * addon);
}


QRectF
NodeGeometry::
boundingRect() const
{
  double addon = 3 * _connectionPointDiameter;

  return QRectF(0 - addon,
                0 - addon,
                _width + 2 * addon,
                _height + 2 * addon);
}


void
NodeGeometry::
recalculateSize()
{
  _height = 0;

  unsigned int step = _entryHeight + _spacing;

  _height += step * _nSinks;

  _height += _spacing;

  _height += step * _nSources;
}


QPointF
NodeGeometry::
connectionPointScenePosition(int index,
                             EndType endType) const
{
  unsigned int step = _entryHeight + _spacing;

  switch (endType)
  {
    case EndType::SOURCE:
    {
      double totalHeight = 0;

      totalHeight += step * _nSinks;
      totalHeight += _spacing;

      totalHeight += step * index;

      // TODO: why?
      totalHeight += (_spacing + _entryHeight) / 2.0;

      double x = _width + _connectionPointDiameter * 1.3;

      return QPointF(x, totalHeight);
      break;
    }

    case EndType::SINK:
    {
      double totalHeight = 0;

      totalHeight += step * index;

      totalHeight += _spacing / 2 +
                     _entryHeight / 2;

      double x = 0.0 - _connectionPointDiameter * 1.3;

      return QPointF(x, totalHeight);
      break;
    }

    default:
      break;
  }

  return QPointF();
}


void setDraggingPosition(QPointF const& pos);
