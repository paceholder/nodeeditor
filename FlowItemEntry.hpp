#ifndef FLOW_ITEM_ENTRY_H
#define FLOW_ITEM_ENTRY_H

#include <QGraphicsObject>

#include <QWeakPointer>

class FlowItem;

class FlowItemEntry: public QGraphicsObject
{
  Q_OBJECT

public:
  enum Type { SOURCE, SINK };

public:
  FlowItemEntry(Type type, QGraphicsItem* parent);

  QRectF
  boundingRect() const override;

  double
  width() const { return _width; }

  double
  height() const { return _height; }

protected:
  void
  paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

private:
  int  _width;
  int  _height;
  Type _type;
};

#endif //  FLOW_ITEM_ENTRY_H
