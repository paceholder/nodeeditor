#ifndef FLOW_ITEM_ENTRY_H
#define FLOW_ITEM_ENTRY_H

#include <QtWidgets/QGraphicsObject>

#include <QtCore/QUuid>
#include <QtCore/QWeakPointer>

class FlowItem;

class FlowItemEntry: public QGraphicsObject
{
  Q_OBJECT

public:
  enum Type { SOURCE, SINK };

public:
  QUuid
  id() const;

  QUuid
  parentID() const;

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
  paint(QPainter*                       painter,
        const QStyleOptionGraphicsItem* option,
        QWidget*                        widget = 0) override;

private:
  QUuid _id;
  Type  _type;
  int   _width;
  int   _height;
};

#endif //  FLOW_ITEM_ENTRY_H
