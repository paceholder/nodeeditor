#ifndef FLOW_ITEM_ENTRY_H
#define FLOW_ITEM_ENTRY_H

#include <QGraphicsObject>

#include <QWeakPointer>

class FlowItem;

class FlowItemEntry: public QGraphicsObject
{
  Q_OBJECT

public:
  FlowItemEntry(QWeakPointer<FlowItem> parent);

private:
  QWeakPointer<FlowItem> _parent;
};

#endif //  FLOW_ITEM_ENTRY_H
