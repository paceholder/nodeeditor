#ifndef FLOW_ITEM_H
#define FLOW_ITEM_H

#include <QGraphicsObject>

class FlowItemEntry;

class FlowItem: public QGraphicsObject
{
  Q_OBJECT

public:
  FlowItem();

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

protected:
  void
  paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

private:
  int _width;
  int _height;
  int _spacing;

  int _connectionPointDiameter;

  QList<FlowItemEntry*> _sourceEntries;
  QList<FlowItemEntry*> _sinkEntries;

private:
  void
  initializeEntries();

  void
  recalculateSize();
};

#endif // FLOW_ITEM_H
