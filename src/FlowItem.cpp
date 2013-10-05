#include "FlowItem.hpp"

#include <QtWidgets/QtWidgets>

#include <iostream>

#include "FlowItemEntry.hpp"
FlowItem::
FlowItem():
  _id(QUuid::createUuid()),
  _width(100),
  _height(150),
  _spacing(10),
  _connectionPointDiameter(12)
{
  setFlag(QGraphicsItem::ItemIsMovable, true);
  setFlag(QGraphicsItem::ItemIsFocusable, true);
}

void
FlowItem::
initializeFlowItem()
{
  initializeEntries();
  embedQWidget();
  recalculateSize();
}

void
FlowItem::
initializeEntries()
{
  FlowItemEntry* entry = new FlowItemEntry(FlowItemEntry::SINK, _id);

  std::cout << "id " << _id.toString().toLocal8Bit().data() << std::endl;

  int height = entry->height();
  _sinkEntries.append(entry);

  std::cout << "create flow item entry " << std::endl;
  entry = new FlowItemEntry(FlowItemEntry::SINK, _id);

  _sinkEntries.append(entry);

  entry = new FlowItemEntry(FlowItemEntry::SINK, _id);

  _sinkEntries.append(entry);

  for (int i = 0; i < _sinkEntries.size(); ++i)
    _sinkEntries[i]->setPos(0, i * (height + _spacing) + _spacing / 2);
}

void
FlowItem::
embedQWidget()
{
  QPushButton* button = new QPushButton(QString("Hello"));

  QGraphicsProxyWidget* proxyWidget = new QGraphicsProxyWidget();

  proxyWidget->setWidget(button);

  button->setVisible(true);
  proxyWidget->setParentItem(this);
}

void
FlowItem::
recalculateSize()
{
  int height = 0;

  if (_sinkEntries.size() > 0)
    height = _sinkEntries[0]->height();

  _height = _sinkEntries.size() * (height + _spacing);
}

QRectF
FlowItem::
boundingRect() const
{
  double addon = 3 * _connectionPointDiameter;
  return QRectF(0 - addon, 0 - addon,
                _width + 2 * addon, _height + 2 * addon);
}

void
FlowItem::
paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
  painter->setPen(Qt::white);
  painter->setBrush(QColor(Qt::darkGray));
  QRectF boundary(0 - _connectionPointDiameter,
                  0 - _connectionPointDiameter,
                  _width + 2 *  _connectionPointDiameter,
                  _height + 2 * _connectionPointDiameter);

  painter->drawRoundedRect(boundary, 10.0, 10.0);

  painter->setBrush(QColor(Qt::gray).darker());

  for (int i = 0; i < _sinkEntries.size(); ++i) {
    double h = _sinkEntries[i]->height();

    double y = (h + _spacing) * i + _spacing / 2 + h / 2;
    double x = 0.0 - _connectionPointDiameter * 1.5;
    painter->drawEllipse(QPointF(x, y),
                         _connectionPointDiameter * 0.8,
                         _connectionPointDiameter * 0.8);
  }

  painter->setPen(Qt::cyan);
  painter->setBrush(Qt::cyan);

  for (int i = 0; i < _sinkEntries.size(); ++i) {
    double h = _sinkEntries[i]->height();

    double y = (h + _spacing) * i + _spacing / 2 + h / 2;
    double x = 0.0 - _connectionPointDiameter * 1.5;
    painter->drawEllipse(QPointF(x, y),
                         _connectionPointDiameter * 0.4,
                         _connectionPointDiameter * 0.4);
  }
}

void
FlowItem::
mousePressEvent(QGraphicsSceneMouseEvent* event)
{
  std::cout << " Block pressed" << std::endl;
  QGraphicsObject::mousePressEvent(event);
}

void
FlowItem::
mouseMoveEvent(QGraphicsSceneMouseEvent* event)
{
  event->ignore();
  QGraphicsObject::mouseMoveEvent(event);
}
