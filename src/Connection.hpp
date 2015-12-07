#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

class FlowItem;

class Connection : public QGraphicsObject
{
  Q_OBJECT

public:
  enum EndType
  {
    SOURCE,
    SINK,
    NONE
  };

public:
  Connection(std::pair<QUuid, int> address,
             EndType dragging);

  void initializeConnection();

  QUuid id();

  void setDragging(EndType dragging);

  EndType dragging() const { return _dragging; }

  QRectF boundingRect() const override;

  void advance(int phase);

  void timerEvent(QTimerEvent* event);

public:
  void connectToFlowItem(std::pair<QUuid, int> address);

  QPointF endPointSceneCoordinate(EndType endType);

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

public slots:
  void onItemMoved();

protected:
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0) override;

private:

  FlowItem* locateFlowItemAt(QPointF const &scenePoint,
                             QTransform const &transform);

private:

  // addressing

  QUuid _id;

  std::pair<QUuid, int> _sourceAddress; // ItemID, entry number
  std::pair<QUuid, int> _sinkAddress;

  QPointF _source;
  QPointF _sink;
  QPointF _lastPoint;

  // state

  EndType _dragging;

  // painting

  double _pointDiameter;

  int _animationPhase;

  double _lineWidth;
};

#endif // CONNECTION_H
