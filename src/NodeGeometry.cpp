#include "NodeGeometry.hpp"

#include <iostream>

#include "EndType.hpp"
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
  , _nSources(dataModel->nSlots(EndType::SOURCE))
  , _nSinks(dataModel->nSlots(EndType::SINK))
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
recalculateSize()
{
  _entryHeight = _fontMetrics.height();

  {
    unsigned int maxNumOfEntries = std::max(_nSinks, _nSources);
    unsigned int step = _entryHeight + _spacing;
    _height = step * maxNumOfEntries;
  }

  auto slotWidth =
    [this](EndType endType, unsigned &width)
    {
      width = 0;
      for (auto i = 0ul; i < _dataModel->nSlots(endType); ++i)
      {
        auto const &name = _dataModel->data(EndType::SINK, i)->name();
        std::cout << "Name: " << name.toLocal8Bit().data() << std::endl;
        width = std::max(unsigned(_fontMetrics.width(name)),
                         width);
      }
    };

  slotWidth(EndType::SINK, _inputSlotWidth);
  slotWidth(EndType::SOURCE, _outputSlotWidth);

  _width = _inputSlotWidth +
           _outputSlotWidth +
           2 * _spacing;
}


void
NodeGeometry::
recalculateSize(QFontMetrics const & fontMetrics)
{
  if (_fontMetrics != fontMetrics)
  {
    _fontMetrics = fontMetrics;

    recalculateSize();
  }
}


QPointF
NodeGeometry::
connectionPointScenePosition(int index,
                             EndType endType,
                             QTransform t) const
{
  unsigned int step = _entryHeight + _spacing;

  QPointF result;

  switch (endType)
  {
    case EndType::SOURCE:
    {
      double totalHeight = 0.0;

      totalHeight += step * index;

      // TODO: why?
      totalHeight += (_spacing + _entryHeight) / 2.0;

      double x = _width + _connectionPointDiameter;

      result = QPointF(x, totalHeight);
      break;
    }

    case EndType::SINK:
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


PortNumber
NodeGeometry::
checkHitScenePoint(EndType endType,
                   QPointF const scenePoint,
                   NodeState const & nodeState,
                   QTransform sceneTransform) const
{
  PortNumber result = PortNumber::INVALID;

  if (endType == EndType::NONE)
    return result;

  double const tolerance = 2.0 * _connectionPointDiameter;

  size_t const nItems = nodeState.getEntries(endType).size();

  for (size_t i = 0; i < nItems; ++i)
  {
    QPointF p = connectionPointScenePosition(i, endType, sceneTransform) - scenePoint;
    auto    distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
    {
      result = PortNumber(i);
      break;
    }
  }

  return result;
}
