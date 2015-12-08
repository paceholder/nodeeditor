#include "Node.hpp"

#include <QtWidgets/QtWidgets>
#include <QtWidgets/QGraphicsEffect>

#include <iostream>
#include <cstdlib>

#include "FlowItemEntry.hpp"
#include "FlowScene.hpp"

Node::
Node()
  : _id(QUuid::createUuid())
  , _width(100)
  , _height(150)
  , _spacing(10)
  , _hovered(false)
  , _connectionPointDiameter(12)
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
                             Connection::EndType endType) const
{

  return connectionPointScenePosition(address.second, endType);
}


QPointF
Node::
connectionPointScenePosition(int index,
                             Connection::EndType endType) const
{

  switch (endType)
  {
    case Connection::SOURCE:
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

    case Connection::SINK:
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
tryConnect(Connection* connection)
{
  std::cout << "TRY CONNECT " << std::endl;

  using EndType = Connection::EndType;

  auto tryConnectToEnd =
    [&](EndType endType)
    {
      auto p = connection->endPointSceneCoordinate(endType);

      int hit = checkHitScenePoint(endType, p);

      auto &entries = getEntryArray(endType);

      bool result = false;

      if (hit >= 0 &&
          entries[hit]->getConnectionID().isNull())
      {
        // can connect
        entries[hit]->setConnectionID(connection->id());

        connect(this, &Node::itemMoved,
                connection, &Connection::onItemMoved);

        auto address = std::make_pair(_id, hit);
        connection->connectToNode(address);

        result = true;
      }

      return result;
    };

  Connection::EndType draggingEnd = connection->dragging();

  if (draggingEnd != EndType::NONE)
  {
    return tryConnectToEnd(draggingEnd);
  }

  return false;
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
      painter->drawEllipse(QPointF(x, y),
                           _connectionPointDiameter * 0.4,
                           _connectionPointDiameter * 0.4);

    totalHeight += h + _spacing;
  }

  totalHeight += _spacing;

  for (size_t i = 0; i < _sourceEntries.size(); ++i)
  {
    double h = _sourceEntries[i]->height();

    double y = totalHeight + (_spacing  + h) / 2;
    double x = _width + _connectionPointDiameter * 1.3;

    if (!_sourceEntries[i]->getConnectionID().isNull())
      painter->drawEllipse(QPointF(x, y),
                           _connectionPointDiameter * 0.4,
                           _connectionPointDiameter * 0.4);

    totalHeight += h + _spacing;
  }
}


// todo make unsigned, define invalid #
int
Node::
checkHitScenePoint(Connection::EndType endType,
                   QPointF const point) const
{

  switch (endType)
  {
    case Connection::EndType::SINK:
      return checkHitSinkScenePoint(point);
      break;

    case Connection::EndType::SOURCE:
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
    QPointF p = connectionPointScenePosition(i, Connection::SINK) - eventPoint;

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
    QPointF p = connectionPointScenePosition(i, Connection::SOURCE) - eventPoint;
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
  int hit = checkHitSinkScenePoint(mapToScene(event->pos()));

  FlowScene &flowScene = FlowScene::instance();

  if (hit >= 0)
  {
    // node's sink has no connection
    if (_sinkEntries[hit]->getConnectionID().isNull())
    {
      auto  address      = std::make_pair(_id, hit);
      QUuid connectionID = flowScene.createConnection(address,
                                                      Connection::SOURCE);

      FlowItemEntry* entry = _sinkEntries[hit];
      entry->setConnectionID(connectionID);
    }
    else
    {
      flowScene.setDraggingConnection(_sinkEntries[hit]->getConnectionID(),
                                      Connection::SINK);
      _sinkEntries[hit]->setConnectionID(QUuid());
    }
  }

  //

  hit = checkHitSourceScenePoint(mapToScene(event->pos()));

  if (hit >= 0)
  {
    if (_sourceEntries[hit]->getConnectionID().isNull())
    {
      auto  address      = std::make_pair(_id, hit);
      QUuid connectionID = flowScene.createConnection(address, Connection::SINK);

      FlowItemEntry* entry = _sourceEntries[hit];
      entry->setConnectionID(connectionID);
    }
    else
    {
      flowScene.setDraggingConnection(_sourceEntries[hit]->getConnectionID(),
                                      Connection::SOURCE);
      _sourceEntries[hit]->setConnectionID(QUuid());
    }
  }

  //event->ignore();

  QGraphicsObject::mousePressEvent(event);
}


void
Node::
mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  if (!FlowScene::instance().isDraggingConnection())
  {
    if (event->lastPos() != event->pos())
      emit itemMoved();
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
  using EndType = Connection::EndType;

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
getEntryArray(Connection::EndType endType)
{
  using EndType = Connection::EndType;

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