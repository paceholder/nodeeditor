#ifndef FLOW_ITEM_H
#define FLOW_ITEM_H

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"

#include "NodeGeometry.hpp"

class FlowItemEntry;

class Node : public QGraphicsObject
{
  Q_OBJECT

public:
  Node();

  void initializeNode();

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

  QPointF connectionPointScenePosition(std::pair<QUuid, int> address,
                                       EndType endType) const;

  QPointF connectionPointScenePosition(int index,
                                       EndType type) const;

  void reactToPossibleConnection(EndType draggingEnd,
                                 QPointF const &scenePoint);

  bool canConnect(EndType draggingEnd, QPointF const &scenePoint);

  std::pair<QUuid, int>
  connect(Connection const* connection,
          EndType draggingEnd,
          int hit);

  std::pair<QUuid, int>
  connect(Connection const* connection,
          EndType draggingEnd,
          QPointF const& scenePoint);

  void disconnect(Connection const* connection,
                  EndType endType,
                  int hit);

signals:
  void itemMoved(QUuid, QPointF const &);

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

  int checkHitScenePoint(EndType endType,
                         QPointF const eventPoint) const;

  int checkHitSinkScenePoint(QPointF const eventPoint) const;

  int checkHitSourceScenePoint(QPointF const eventPoint) const;

private:

  std::vector<FlowItemEntry*>& getEntryArray(EndType endType);

private:
  // addressing

  QUuid _id;

  // structure

  std::vector<FlowItemEntry*> _sourceEntries;
  std::vector<FlowItemEntry*> _sinkEntries;

  // painting

  NodeGeometry _nodeGeometry;
};

#endif // FLOW_ITEM_H
