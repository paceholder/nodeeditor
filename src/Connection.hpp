#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

class Connection: public QGraphicsObject
{
  Q_OBJECT

public:
  enum Dragging { SOURCE, SINK, NONE };

  QUuid
  id() { return _id; }

public:
  Connection(QUuid    flowItemID,
             int      entryNumber,
             Dragging dragging);

  void
  initializeConnection();

  QRectF
  boundingRect() const override;

  void
  advance(int phase);

  void
  timerEvent(QTimerEvent* event);

  void
  mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void
  mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void
  mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

public slots:
  void
  onItemMoved();

protected:
  void
  paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

  /**
   * dataTransfer
   * */

private:
  QUuid _id;
  QUuid _flowItemSourceID;
  QUuid _flowItemSinkID;

  int _sourceEntryNumber;
  int _sinkEntryNumber;

  QPointF _source;
  QPointF _sink;
  QPointF _lastPoint;

  Dragging _dragging;

  double _pointDiameter;

  int _animationPhase;
};

#endif // CONNECTION_H
