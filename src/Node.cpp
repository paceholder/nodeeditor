#include "Node.hpp"

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include <iostream>
#include <cstdlib>

#include "ConnectionGraphicsObject.hpp"
#include "FlowItemEntry.hpp"
#include "FlowScene.hpp"

Node::
Node()
  : _id(QUuid::createUuid())
  , _width(100)
  , _height(150)
  , _spacing(10)
  , _hovered(false)
  , _connectionPointDiameter(10)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);

  auto effect = new QGraphicsDropShadowEffect;
  effect->setOffset(4, 4);
  effect->setBlurRadius(20);
  effect->setColor(QColor(Qt::gray).darker(800));

  setGraphicsEffect(effect);
}


void
Node::
initializeNode()
{
  setAcceptHoverEvents(true);

  initializeEntries();

  //embedQWidget();

  recalculateSize();
}


void
Node::
embedQWidget()
{
  QPushButton* button = new QPushButton(QString("Hello"));

  QGraphicsProxyWidget* proxyWidget = new QGraphicsProxyWidget();

  proxyWidget->setWidget(button);

  button->setVisible(true);
  proxyWidget->setParentItem(this);
}


void
Node::
recalculateSize()
{
  int totalHeight = 0;

  for (size_t i = 0; i < _sinkEntries.size(); ++i)
    totalHeight += _sinkEntries[i]->height() + _spacing;

  totalHeight += _spacing;

  for (size_t i = 0; i < _sourceEntries.size(); ++i)
    totalHeight += _sourceEntries[i]->height() + _spacing;

  _height = totalHeight;
}


QRectF
Node::
boundingRect() const
{
  double addon = 3 * _connectionPointDiameter;

  return QRectF(0 - addon,
                0 - addon,
                _width + 2 * addon,
                _height + 2 * addon);
}


QPointF
Node::
connectionPointScenePosition(std::pair<QUuid, int> address,
                             EndType endType) const
{
  return connectionPointScenePosition(address.second, endType);
}


QPointF
Node::
connectionPointScenePosition(int index,
                             EndType endType) const
{

  switch (endType)
  {
    case EndType::SOURCE:
    {
      double totalHeight = 0;

      for (size_t i = 0; i < _sinkEntries.size(); ++i)
        totalHeight += _sinkEntries[i]->height() + _spacing;

      totalHeight += _spacing;

      for (int i = 1; i <= index; ++i)
        totalHeight += _sourceEntries[i]->height() + _spacing;

      totalHeight += (_spacing  + _sourceEntries[index]->height()) / 2.0;
      double x = _width + _connectionPointDiameter * 1.3;

      return mapToScene(QPointF(x, totalHeight));
      break;
    }

    case EndType::SINK:
    {
      double totalHeight = 0;

      for (int i = 1; i <= index; ++i)
        totalHeight += _sinkEntries[i]->height() + _spacing;

      totalHeight += _spacing / 2 + _sinkEntries[index]->height() / 2;
      double x = 0.0 - _connectionPointDiameter * 1.3;

      return mapToScene(QPointF(x, totalHeight));
      break;
    }

    default:
      break;
  }

  return QPointF();
}


bool
Node::
canConnect(EndType draggingEnd, QPointF const &scenePoint)
{
  auto &entries = getEntryArray(draggingEnd);
  int  hit      = checkHitScenePoint(draggingEnd, scenePoint);

  return (hit >= 0 &&
          entries[hit]->getConnectionID().isNull());
}


std::pair<QUuid, int>
Node::
connect(Connection const* connection,
        EndType draggingEnd,
        int hit)
{
  auto &entries = getEntryArray(draggingEnd);

  entries[hit]->setConnectionID(connection->id());

  QObject::connect(this, &Node::itemMoved,
                   connection->getConnectionGraphicsObject(),
                   &ConnectionGraphicsObject::onItemMoved);

  connection->getConnectionGraphicsObject()->stackBefore(this);

  auto address = std::make_pair(_id, hit);

  return address;
}


std::pair<QUuid, int>
Node::
connect(Connection const* connection,
        EndType draggingEnd,
        QPointF const& scenePoint)
{
  int hit = checkHitScenePoint(draggingEnd, scenePoint);

  return connect(connection, draggingEnd, hit);
}


void
Node::
paint(QPainter* painter,
      QStyleOptionGraphicsItem const*,
      QWidget* )
{
  if (_hovered)
  {
    QPen p(Qt::white, 2.0);
    painter->setPen(p);
  }
  else
  {
    QPen p(Qt::white, 1.5);
    painter->setPen(p);
  }

  painter->setBrush(QColor(Qt::darkGray));

  QRectF boundary(0 - _connectionPointDiameter,
                  0 - _connectionPointDiameter,
                  _width + 2 *  _connectionPointDiameter,
                  _height + 2 * _connectionPointDiameter);

  constexpr double radius = 3.0;

  painter->drawRoundedRect(boundary, radius, radius);

  drawConnectionPoints(painter);

  drawFilledConnectionPoints(painter);
}


void
Node::
drawConnectionPoints(QPainter* painter)
{
  painter->setBrush(QColor(Qt::gray).darker());
  double totalHeight = 0;

  for (size_t i = 0; i < _sinkEntries.size(); ++i)
  {
    double h = _sinkEntries[i]->height();

    double y = totalHeight + (_spacing  + h) / 2;
    double x = 0.0 - _connectionPointDiameter * 1.3;
    painter->drawEllipse(QPointF(x, y),
                         _connectionPointDiameter * 0.6,
                         _connectionPointDiameter * 0.6);

    totalHeight += h + _spacing;
  }

  totalHeight += _spacing;

  for (size_t i = 0; i < _sourceEntries.size(); ++i)
  {
    double h = _sourceEntries[i]->height();

    double y = totalHeight + (_spacing  + h) / 2;
    double x = _width + _connectionPointDiameter * 1.3;
    painter->drawEllipse(QPointF(x, y),
                         _connectionPointDiameter * 0.6,
                         _connectionPointDiameter * 0.6);

    totalHeight += h + _spacing;
  }
}


void
Node::
drawFilledConnectionPoints(QPainter* painter)
{
  painter->setPen(Qt::cyan);
  painter->setBrush(Qt::cyan);

  double totalHeight = 0;

  for (size_t i = 0; i < _sinkEntries.size(); ++i)
  {
    double h = _sinkEntries[i]->height();

    double y = totalHeight + (_spacing  + h) / 2;
    double x = 0.0 - _connectionPointDiameter * 1.3;

    if (!_sinkEntries[i]->getConnectionID().isNull())
    {

      painter->drawEllipse(QPointF(x, y),
                           _connectionPointDiameter * 0.4,
                           _connectionPointDiameter * 0.4);
    }

    totalHeight += h + _spacing;
  }

  totalHeight += _spacing;

  for (size_t i = 0; i < _sourceEntries.size(); ++i)
  {
    double h = _sourceEntries[i]->height();

    double y = totalHeight + (_spacing  + h) / 2;
    double x = _width + _connectionPointDiameter * 1.3;

    if (!_sourceEntries[i]->getConnectionID().isNull())
    {
      painter->drawEllipse(QPointF(x, y),
                           _connectionPointDiameter * 0.4,
                           _connectionPointDiameter * 0.4);
    }

    totalHeight += h + _spacing;
  }
}


// todo make unsigned, define invalid #
int
Node::
checkHitScenePoint(EndType endType,
                   QPointF const point) const
{

  switch (endType)
  {
    case EndType::SINK:
      return checkHitSinkScenePoint(point);
      break;

    case EndType::SOURCE:
      return checkHitSourceScenePoint(point);
      break;

    default:
      break;
  }

  return -1;
}


int
Node::
checkHitSinkScenePoint(const QPointF eventPoint) const
{
  int result = -1;

  double tolerance = 1.0 * _connectionPointDiameter;

  for (size_t i = 0; i < _sinkEntries.size(); ++i)
  {
    QPointF p = connectionPointScenePosition(i, EndType::SINK) - eventPoint;

    auto distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
      result = i;
  }

  return result;
}


int
Node::
checkHitSourceScenePoint(const QPointF eventPoint) const
{
  int result = -1;

  double tolerance = 1.0 * _connectionPointDiameter;

  for (size_t i = 0; i < _sourceEntries.size(); ++i)
  {
    QPointF p = connectionPointScenePosition(i, EndType::SOURCE) - eventPoint;
    auto    distance = std::sqrt(QPointF::dotProduct(p, p));

    if (distance < tolerance)
      result = i;
  }

  return result;
}


void
Node::
mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  auto clickEnd =
    [&](EndType endToCheck)
    {
      int hit = checkHitScenePoint(endToCheck,
                                   event->scenePos());

      FlowScene &flowScene = FlowScene::instance();

      if (hit >= 0)
      {
        auto& entries = getEntryArray(endToCheck);

        // node's sink has no connection

        QUuid const id = entries[hit]->getConnectionID();

        if (id.isNull())
        {
          // todo add to FlowScene
          auto connection = flowScene.createConnection();

          connect(connection, endToCheck, hit);

          auto address = std::make_pair(_id, hit);

          auto conPoint = connectionPointScenePosition(address, endToCheck);

          connection->setDraggingEnd(endToCheck);
          connection->connectToNode(address, conPoint);

          connection->setDraggingEnd(oppositeEnd(endToCheck));
        }
        else
        {
          auto connection = flowScene.getConnection(id);

          QObject::disconnect(this, &Node::itemMoved,
                              connection->getConnectionGraphicsObject(),
                              &ConnectionGraphicsObject::onItemMoved);

          connection->setDraggingEnd(endToCheck);
          entries[hit]->setConnectionID(QUuid());
        }
      }
    };

  clickEnd(EndType::SINK);
  clickEnd(EndType::SOURCE);

  //event->ignore();
}


void
Node::
mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  QPointF d = event->pos() - event->lastPos();

  if (!FlowScene::instance().isDraggingConnection())
  {
    if (event->lastPos() != event->pos())
      emit itemMoved(_id, d);
  }

  //event->ignore();

  QGraphicsObject::mouseMoveEvent(event);
}


void
Node::
hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
  _hovered = true;
  update();
  event->accept();
}


void
Node::
hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
  _hovered = false;
  update();
  event->accept();
}


void
Node::
initializeEntries()
{
  unsigned int n = std::rand() % 4 + 2;

  for (auto i = 0ul; i < n; ++i)
  {
    FlowItemEntry* entry =
      new FlowItemEntry(EndType::SINK, _id);

    _sinkEntries.push_back(entry);
  }

  int totalHeight = 0;
  for (size_t i = 0; i < _sinkEntries.size(); ++i)
  {
    _sinkEntries[i]->setPos(0, totalHeight + _spacing / 2);
    totalHeight += _sinkEntries[i]->height() + _spacing;
  }

  ////////////////////////////

  n = std::rand() % 4 + 2;

  for (auto i = 0ul; i < n; ++i)
  {
    FlowItemEntry* entry =
      new FlowItemEntry(EndType::SOURCE, _id);

    _sourceEntries.push_back(entry);
  }

  totalHeight += _spacing;

  for (size_t i = 0; i < _sourceEntries.size(); ++i)
  {
    _sourceEntries[i]->setPos(0, totalHeight + _spacing / 2);
    totalHeight += _sourceEntries[i]->height() + _spacing;
  }
}


std::vector<FlowItemEntry*>&
Node::
getEntryArray(EndType endType)
{
  switch (endType)
  {
    case EndType::SOURCE:
      return _sourceEntries;
      break;

    case EndType::SINK:
      return _sinkEntries;
      break;

    default:
      break;
  }
}
