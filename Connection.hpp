#ifndef CONNECTION_H
#define CONNECTION_H

#include <QGraphicsObject>

class ConnectionPoint: public QGraphicsObject
{
  Q_OBJECT

public:
  ConnectionPoint(QGraphicsItem* parent):
    QGraphicsObject(parent) {}

  QRectF
  boundingRect() const override;

protected:
  void
  paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

  void
  mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  // void
  // mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
};

class Connection: public QGraphicsObject
{
  Q_OBJECT

public:
  Connection(int BlockOutID, int BlockInID);

  QRectF
  boundingRect() const override;

  void
  advance(int phase);

  void
  timerEvent(QTimerEvent* event);

  void
  mousePressEvent(QGraphicsSceneMouseEvent* event) override;

protected:
  void
  paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

  /**
   * dataTransfer
   * */

private:
  ConnectionPoint* source;
  ConnectionPoint* sink;

  int _animationPhase;
};

#endif // CONNECTION_H
