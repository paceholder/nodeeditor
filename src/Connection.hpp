#ifndef CONNECTION_H
#define CONNECTION_H

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

class Node;

//------------------------------------------------------------------------------

enum class EndType
{
  NONE,
  SOURCE,
  SINK
};

//------------------------------------------------------------------------------

struct ConnectionGeometry
{
  ConnectionGeometry()
    : source(10, 10)
    , sink(100, 100)
    , pointDiameter(10)
    , animationPhase(0)
    , lineWidth(3.0)
  {}

  inline
  QPointF&getEndPoint(EndType endType)
  {
    Q_ASSERT(endType != EndType::NONE);

    return (endType == EndType::SOURCE) ? source : sink;
  }

  // local object coordinates
  QPointF source;
  QPointF sink;

  double pointDiameter;

  int animationPhase;

  double lineWidth;
};

//------------------------------------------------------------------------------

class Connection;

class ConnectionGraphicsObject : public QGraphicsObject
{
public:
  ConnectionGraphicsObject(Connection& connection,
                           ConnectionGeometry& connectionGeometry)
    : _connection(connection)
    , _connectionGeometry(connectionGeometry)
  {}

protected:
  void mousePressEvent(QGraphicsSceneMouseEvent* event) override
  { QGraphicsObject::mousePressEvent(event); }

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  Connection& _connection;
  ConnectionGeometry& _connectionGeometry;
};

//------------------------------------------------------------------------------

class Connection
{
  Q_OBJECT

public:

public:
  Connection(std::pair<QUuid, int> address,
             EndType dragging);

  void initializeConnection();

  QUuid id() const;

  void setDraggingEnd(EndType dragging);
  EndType draggingEnd() const { return _draggingEnd; }

  QRectF boundingRect() const override;

  void advance(int phase);

  void timerEvent(QTimerEvent* event);

public:
  void connectToNode(std::pair<QUuid, int> address);

  QPointF endPointSceneCoordinate(EndType endType);

protected:

public slots:
  void onItemMoved();

protected:
  void paint(QPainter* painter,
             const QStyleOptionGraphicsItem* option,
             QWidget* widget = 0) override;

private:

  Node* locateNodeAt(QGraphicsSceneMouseEvent* event);

private:

  // addressing

  QUuid _id;

  std::pair<QUuid, int> _sourceAddress; // ItemID, entry number
  std::pair<QUuid, int> _sinkAddress;

  // state

  EndType _draggingEnd;

  // painting

  ConnectionGeometry connectionGeometry;
};

#endif // CONNECTION_H
