#ifndef FLOW_ITEM_ENTRY_H
#define FLOW_ITEM_ENTRY_H

#include <iostream>

#include <QtWidgets/QGraphicsObject>
#include <QtWidgets/QGraphicsSceneMoveEvent>

#include <QtCore/QUuid>
#include <QtCore/QWeakPointer>

#include "Connection.hpp"

class Node;

class FlowItemEntry : public QGraphicsObject
{
  Q_OBJECT

public:
  QUuid id() const;

  QUuid parentID() const;

public:
  FlowItemEntry(EndType type,
                QUuid parentID,
                QString name = QString("Entry"),
                QUuid connectionID = QUuid());

  QRectF boundingRect() const override;

  double width() const { return _width; }

  double height() const { return _height; }

  void setConnectionID(QUuid connectionID);

  QUuid getConnectionID() const;

protected: // events

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override
  {
    (void)event;

    //std::cout << "PRESS CHILD" << std::endl;
  }

  void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override
  {
    //double const x = event->pos().x();
    //double const y = event->pos().y();
    //std::cout << "Mouse Event Item " << x << ", " << y << std::endl;

    event->ignore();
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

  EndType _type;

  QString _name;

  // painting

  int _width;
  int _height;
};

#endif //  FLOW_ITEM_ENTRY_H
