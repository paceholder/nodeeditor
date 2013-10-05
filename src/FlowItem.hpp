#ifndef FLOW_ITEM_H
#define FLOW_ITEM_H

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

class FlowItemEntry;

class FlowItem: public QGraphicsObject
{
  Q_OBJECT

public:
  FlowItem();

  void
  initializeFlowItem();

  QRectF
  boundingRect() const override;

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
  QUuid
  id();

  QPointF
  sourcePointPos(int index);

  QPointF
  sinkPointPos(int index);

protected:
  void
  paint(QPainter*                       painter,
        const QStyleOptionGraphicsItem* option,
        QWidget*                        widget = 0) override;

  void
  mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void
  mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

private:
  QUuid _id;
  int   _width;
  int   _height;
  int   _spacing;

  int _connectionPointDiameter;

  QList<FlowItemEntry*> _sourceEntries;
  QList<FlowItemEntry*> _sinkEntries;

private:
  void
  initializeEntries();

  void
  embedQWidget();

  void
  recalculateSize();
};

#endif // FLOW_ITEM_H
