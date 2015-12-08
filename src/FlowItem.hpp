#ifndef FLOW_ITEM_H
#define FLOW_ITEM_H

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"

class FlowItemEntry;

class FlowItem : public QGraphicsObject
{
  Q_OBJECT

public:
  FlowItem();

  void initializeFlowItem();

  QRectF boundingRect() const override;

  /* *
   * EVENTS
   *
   * mouseOverIn
   * mouseOverOut
   *
   * mouseReleaseIn
   * mouseReleaseOut
   *
   * mousePressIn
   * mousePressOut
   *
   * */

public:
  QUuid id() { return _id; }

  QPointF connectionPointPosition(std::pair<QUuid, int> address,
                                  Connection::EndType endType) const;

  QPointF connectionPointPosition(int index,
                                  Connection::EndType type) const;

  bool tryConnect(Connection* connection);

signals:
  void itemMoved();

protected:
  void paint(QPainter*                       painter,
             QStyleOptionGraphicsItem const* option,
             QWidget*                        widget = 0) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  void initializeEntries();

  void embedQWidget();

  void recalculateSize();

  int checkHitPoint(Connection::EndType endType,
                    QPointF const eventPoint) const;

  int checkHitSinkPoint(QPointF const eventPoint) const;

  int checkHitSourcePoint(QPointF const eventPoint) const;

  void drawConnectionPoints(QPainter* painter);

  void drawFilledConnectionPoints(QPainter* painter);

private:

  std::vector<FlowItemEntry*>& getEntryArray(Connection::EndType endType);

private:
  // addressing

  QUuid _id;

  // structure

  std::vector<FlowItemEntry*> _sourceEntries;
  std::vector<FlowItemEntry*> _sinkEntries;

  // painting

  int _width;
  int _height;
  int _spacing;

  bool _hovered;

  int _connectionPointDiameter;
};

#endif // FLOW_ITEM_H
