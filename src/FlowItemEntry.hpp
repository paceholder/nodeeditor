#ifndef FLOW_ITEM_ENTRY_H
#define FLOW_ITEM_ENTRY_H

#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsSceneMoveEvent>

#include <QtCore/QUuid>
#include <QtCore/QWeakPointer>


#include <iostream>

class FlowItem;

class FlowItemEntry : public QGraphicsObject
{
  Q_OBJECT

public:
  enum Type { SOURCE, SINK };

public:
  QUuid id() const;

  QUuid parentID() const;

public:
  FlowItemEntry(Type type,
                QUuid parentID,
                QString name = QString("Entry"),
                QUuid connectionID = QUuid());

  QRectF boundingRect() const override;

  double width() const { return _width; }

  double height() const { return _height; }

  void setConnectionID(QUuid connectionID);

  QUuid getConnectionID() const;

protected: // events
  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override
  {
    double const x = event->pos().x();
    double const y = event->pos().y();
    std::cout << "Mouse Event Item " << x << ", " << y << std::endl;
  }

protected:
  void paint(QPainter*                       painter,
             QStyleOptionGraphicsItem const* option,
             QWidget*                        widget = 0) override;

private:
  // addressing

  QUuid _id;

  QUuid _parentID;
  QUuid _connectionID;

  // structure

  Type _type;

  QString _name;

  // painting

  int _width;
  int _height;
};

#endif //  FLOW_ITEM_ENTRY_H
