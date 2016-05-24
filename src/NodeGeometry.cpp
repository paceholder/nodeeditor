#include "NodeGeometry.hpp"

#include <iostream>

#include "PortType.hpp"
#include "NodeState.hpp"
#include "NodeDataModel.hpp"

NodeGeometry::
NodeGeometry(std::unique_ptr<NodeDataModel> const &dataModel)
  : _width(100)
  , _height(150)
  , _inputSlotWidth(70)
  , _outputSlotWidth(70)
  , _entryHeight(20)
  , _spacing(20)
  , _connectionPointDiameter(8)
  , _hovered(false)
  , _nSources(dataModel->nSlots(PortType::OUT))
  , _nSinks(dataModel->nSlots(PortType::IN))
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

  auto slotWidth =
    [this](PortType portType)
    {
      unsigned width = 0;

      for (auto i = 0ul; i < _dataModel->nSlots(portType); ++i)
      {
        auto const &name = _dataModel->data(PortType::IN, i)->name();
        width = std::max(unsigned(_fontMetrics.width(name)),
                         width);
      }

      return width;
    };

  _inputSlotWidth  = slotWidth(PortType::IN);
  _outputSlotWidth = slotWidth(PortType::OUT);

  _width = _inputSlotWidth +
           _outputSlotWidth +
           2 * _spacing;
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

  switch (portType)
  {
    case PortType::OUT:
    {
      double totalHeight = 0.0;

      totalHeight += step * index;

      // TODO: why?
      totalHeight += (_spacing + _entryHeight) / 2.0;

      double x = _width + _connectionPointDiameter;

      result = QPointF(x, totalHeight);
      break;
    }

    case PortType::IN:
    {
      double totalHeight = 0;

      totalHeight += step * index;

      totalHeight += _spacing / 2 +
                     _entryHeight / 2;

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

  if (portType == PortType::NONE)
    return result;

  double const tolerance = 2.0 * _connectionPointDiameter;

  size_t const nItems = _dataModel->nSlots(portType);

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
