#include "NodeGeometry.hpp"

#include "PortType.hpp"
#include "NodeState.hpp"
#include "NodeDataModel.hpp"
#include "Node.hpp"
#include "NodeGraphicsObject.hpp"
#include "StyleCollection.hpp"

#include <QtGlobal>

#include <iostream>
#include <cmath>

using QtNodes::NodeGeometry;
using QtNodes::NodeDataModel;
using QtNodes::PortIndex;
using QtNodes::PortType;
using QtNodes::Node;

NodeGeometry::
NodeGeometry(std::unique_ptr<NodeDataModel> const &dataModel)
  : _width(100)
  , _height(150)
  , _inputPortWidth(70)
  , _outputPortWidth(70)
  , _entryHeight(20)
  , _spacing(20)
  , _hovered(false)
  , _nSources(dataModel->nPorts(PortType::Out))
  , _nSinks(dataModel->nPorts(PortType::In))
  , _draggingPos(-1000, -1000)
  , _dataModel(dataModel)
  , _fontMetrics(QFont())
  , _boldFontMetrics(QFont())
{
  QFont f; f.setBold(true);

  _boldFontMetrics = QFontMetrics(f);
}

unsigned int
NodeGeometry::nSources() const
{
  return _dataModel->nPorts(PortType::Out);
}

unsigned int
NodeGeometry::nSinks() const
{
  return _dataModel->nPorts(PortType::In);
}

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
  auto const &nodeStyle = StyleCollection::nodeStyle();

  double addon = 4 * nodeStyle.ConnectionPointDiameter;

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

  _height += captionHeight();

  _inputPortWidth  = portWidth(PortType::In);
  _outputPortWidth = portWidth(PortType::Out);

  _width = _inputPortWidth +
           _outputPortWidth +
           2 * _spacing;

  if (auto w = _dataModel->embeddedWidget())
  {
    _width += w->width();
  }

  _width = std::max(_width, captionWidth());

  if (_dataModel->validationState() != NodeValidationState::Valid)
  {
    _width   = std::max(_width, validationWidth());
    _height += validationHeight() + _spacing;
  }
}


void
NodeGeometry::
recalculateSize(QFont const & font) const
{
  QFontMetrics fontMetrics(font);
  QFont boldFont = font;

  boldFont.setBold(true);

  QFontMetrics boldFontMetrics(boldFont);

  if (_boldFontMetrics != boldFontMetrics)
  {
    _fontMetrics     = fontMetrics;
    _boldFontMetrics = boldFontMetrics;

    recalculateSize();
  }
}


QPointF
NodeGeometry::
portScenePosition(PortIndex index,
                  PortType portType,
                  QTransform const & t) const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();

  unsigned int step = _entryHeight + _spacing;

  QPointF result;

  double totalHeight = 0.0;

  totalHeight += captionHeight();

  totalHeight += step * index;

  // TODO: why?
  totalHeight += step / 2.0;

  switch (portType)
  {
    case PortType::Out:
    {
      double x = _width + nodeStyle.ConnectionPointDiameter;

      result = QPointF(x, totalHeight);
      break;
    }

    case PortType::In:
    {
      double x = 0.0 - nodeStyle.ConnectionPointDiameter;

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
                   QTransform const & sceneTransform) const
{
  auto const &nodeStyle = StyleCollection::nodeStyle();

  PortIndex result = INVALID;

  if (portType == PortType::None)
    return result;

  double const tolerance = 2.0 * nodeStyle.ConnectionPointDiameter;

  unsigned int const nItems = _dataModel->nPorts(portType);

  for (unsigned int i = 0; i < nItems; ++i)
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
    if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag)
    {
      // If the widget wants to use as much vertical space as possible, place it immediately after the caption.
      return QPointF(_spacing + portWidth(PortType::In), captionHeight());
    }
    else
    {
      if (_dataModel->validationState() != NodeValidationState::Valid)
      {
        return QPointF(_spacing + portWidth(PortType::In),
                      (captionHeight() + _height - validationHeight() - _spacing - w->height()) / 2.0);
      }

      return QPointF(_spacing + portWidth(PortType::In), 
                    (captionHeight() + _height - w->height()) / 2.0);
    }
  }
  return QPointF();
}

int
NodeGeometry::
equivalentWidgetHeight() const
{
  if (_dataModel->validationState() != NodeValidationState::Valid)
  {
    return height() - captionHeight() + validationHeight();
  }

  return height() - captionHeight();
}

unsigned int
NodeGeometry::
captionHeight() const
{
  if (!_dataModel->captionVisible())
    return 0;

  QString name = _dataModel->caption();

  return _boldFontMetrics.boundingRect(name).height();
}


unsigned int
NodeGeometry::
captionWidth() const
{
  if (!_dataModel->captionVisible())
    return 0;

  QString name = _dataModel->caption();

  return _boldFontMetrics.boundingRect(name).width();
}


unsigned int
NodeGeometry::
validationHeight() const
{
  QString msg = _dataModel->validationMessage();

  return _boldFontMetrics.boundingRect(msg).height();
}


unsigned int
NodeGeometry::
validationWidth() const
{
  QString msg = _dataModel->validationMessage();

  return _boldFontMetrics.boundingRect(msg).width();
}


QPointF
NodeGeometry::
calculateNodePositionBetweenNodePorts(PortIndex targetPortIndex, PortType targetPort, Node* targetNode, 
                                      PortIndex sourcePortIndex, PortType sourcePort, Node* sourceNode, 
                                      Node& newNode)
{
  //Calculating the nodes position in the scene. It'll be positioned half way between the two ports that it "connects". 
  //The first line calculates the halfway point between the ports (node position + port position on the node for both nodes averaged).
  //The second line offsets this coordinate with the size of the new node, so that the new nodes center falls on the originally
  //calculated coordinate, instead of it's upper left corner.
  auto converterNodePos = (sourceNode->nodeGraphicsObject().pos() + sourceNode->nodeGeometry().portScenePosition(sourcePortIndex, sourcePort) +
    targetNode->nodeGraphicsObject().pos() + targetNode->nodeGeometry().portScenePosition(targetPortIndex, targetPort)) / 2.0f;
  converterNodePos.setX(converterNodePos.x() - newNode.nodeGeometry().width() / 2.0f);
  converterNodePos.setY(converterNodePos.y() - newNode.nodeGeometry().height() / 2.0f);
  return converterNodePos;
}


unsigned int
NodeGeometry::
portWidth(PortType portType) const
{
  unsigned width = 0;

  for (auto i = 0ul; i < _dataModel->nPorts(portType); ++i)
  {
    QString name;

    if (_dataModel->portCaptionVisible(portType, i))
    {
      name = _dataModel->portCaption(portType, i);
    }
    else
    {
      name = _dataModel->dataType(portType, i).name;
    }

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    width = std::max(unsigned(_fontMetrics.horizontalAdvance(name)),
                     width);
#else
    width = std::max(unsigned(_fontMetrics.width(name)),
                     width);
#endif
  }

  return width;
}
