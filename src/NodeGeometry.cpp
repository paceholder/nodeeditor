#include "NodeGeometry.hpp"

#include <iostream>
#include <cmath>

#include "PortType.hpp"
#include "NodeState.hpp"
#include "NodeDataModel.hpp"

NodeGeometry::
NodeGeometry(std::unique_ptr<NodeDataModel> const &dataModel)
  : _width(100)
  , _height(150)
  , _inputPortWidth(70)
  , _outputPortWidth(70)
  , _entryHeight(20)
  , _spacing(20)
  , _connectionPointDiameter(8)
  , _hovered(false)
  , _nSources(dataModel->nPorts(PortType::Out))
  , _nSinks(dataModel->nPorts(PortType::In))
  , _draggingPos(-1000, -1000)
  , _opacity(0.80)
  , _dataModel(dataModel)
  , _fontMetrics(QFont())
{}

QRectF
NodeGeometry::
entryBoundingRect() const
{
  double const addon = 0.0;

  return QRectF(0 - addon,
                0 - addon,
                _entryWidth + 2 * addon,
                _entryHeight + 2 * addon);
}


QRectF
NodeGeometry::
boundingRect() const
{
  double addon = 4 * _connectionPointDiameter;

  return QRectF(0 - addon,
                0 - addon,
                _width + 2 * addon,
                _height + 2 * addon);
}


void
NodeGeometry::
recalculateSize() const
{
  _entryHeight = _fontMetrics.height();

  {
    unsigned int maxNumOfEntries = std::max(_nSinks, _nSources);
    unsigned int step = _entryHeight + _spacing;
    _height = step * maxNumOfEntries;
  }

  if (auto w = _dataModel->embeddedWidget())
  {
    _height = std::max(_height, static_cast<unsigned>(w->height()));
  }

  _height += nameHeight();

  _inputPortWidth  = portWidth(PortType::In);
  _outputPortWidth = portWidth(PortType::Out);

  _width = _inputPortWidth +
           _outputPortWidth +
           2 * _spacing;

  if (auto w = _dataModel->embeddedWidget())
  {
    _width += w->width();
  }
}


void
NodeGeometry::
recalculateSize(QFontMetrics const & fontMetrics) const
{
  if (_fontMetrics != fontMetrics)
  {
    _fontMetrics = fontMetrics;

    recalculateSize();
  }
}


QPointF
NodeGeometry::
portScenePosition(int index,
                  PortType portType,
                  QTransform t) const
{
  unsigned int step = _entryHeight + _spacing;

  QPointF result;

  double totalHeight = 0.0;

  totalHeight += nameHeight();

  totalHeight += step * index;

  // TODO: why?
  totalHeight += step / 2.0;

  switch (portType)
  {
    case PortType::Out:
    {
      double x = _width + _connectionPointDiameter;

      result = QPointF(x, totalHeight);
      break;
    }

    case PortType::In:
    {
      double x = 0.0 - _connectionPointDiameter;

      result = QPointF(x, totalHeight);
      break;
    }

    default:
      break;
  }

  return t.map(result);
}


PortIndex
NodeGeometry::
checkHitScenePoint(PortType portType,
                   QPointF const scenePoint,
                   QTransform sceneTransform) const
{
  PortIndex result = INVALID;

  if (portType == PortType::None)
    return result;

  double const tolerance = 2.0 * _connectionPointDiameter;

  size_t const nItems = _dataModel->nPorts(portType);

  for (size_t i = 0; i < nItems; ++i)
  {
    auto pp = portScenePosition(i, portType, sceneTransform);

    QPointF p = pp - scenePoint;
    auto    distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
    {
      result = PortIndex(i);
      break;
    }
  }

  return result;
}


QRect
NodeGeometry::
resizeRect() const
{
  unsigned int rectSize = 7;

  return QRect(_width - rectSize,
               _height - rectSize,
               rectSize,
               rectSize);
}


QPointF
NodeGeometry::
widgetPosition() const
{
  if (auto w = _dataModel->embeddedWidget())
  {

    return QPointF(_spacing + portWidth(PortType::In),
                   (nameHeight() + _height - w->height()) / 2.0);
  }

  return QPointF();
}


unsigned int
NodeGeometry::
nameHeight() const
{
  if (!_dataModel->captionVisible())
    return 0;

  QString name = _dataModel->caption();

  return _fontMetrics.boundingRect(name).height();
}


unsigned int
NodeGeometry::
portWidth(PortType portType) const
{
  unsigned width = 0;

  for (auto i = 0ul; i < _dataModel->nPorts(portType); ++i)
  {
    auto const &name = _dataModel->dataType(PortType::In, i).name;
    width = std::max(unsigned(_fontMetrics.width(name)),
                     width);
  }

  return width;
}
