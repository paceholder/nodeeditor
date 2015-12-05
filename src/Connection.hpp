#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

class Connection : public QGraphicsObject
{
  Q_OBJECT

public:
  enum Dragging { SOURCE, SINK, NONE };

public:
  Connection(QUuid flowItemID,
             int entryNumber,
             Dragging dragging);

  void initializeConnection();

  QUuid id();

  void setDragging(Dragging dragging);

  QRectF boundingRect() const override;

  void advance(int phase);

  void timerEvent(QTimerEvent* event);


protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

public slots:
  void onItemMoved();

protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

private:

  // addressing

  QUuid _id;
  QUuid _flowItemSourceID;
  QUuid _flowItemSinkID;

  int _sourceEntryNumber;
  int _sinkEntryNumber;

  QPointF _source;
  QPointF _sink;
  QPointF _lastPoint;

  // state

  Dragging _dragging;

  // painting

  double _pointDiameter;

  int _animationPhase;

  double _lineWidth;
};

#endif // CONNECTION_H
