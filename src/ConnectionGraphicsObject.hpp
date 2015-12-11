#ifndef CONNECTION_GRAPHICS_OBJECT_H
#define CONNECTION_GRAPHICS_OBJECT_H

#include <QtWidgets/QGraphicsObject>

class QGraphicsSceneMouseEvent;

class Connection;
class ConnectionGeometry;
class ConnectionPainter;

class ConnectionGraphicsObject
  : public QGraphicsObject
{
  Q_OBJECT

public:

  ConnectionGraphicsObject(Connection& connection,
                           ConnectionGeometry& connectionGeometry,
                           ConnectionPainter const& connectionPainter);

public:

  QRectF boundingRect() const override;

public slots:
  void onItemMoved(QUuid id, QPointF const &offset);

  QPainterPath shape() const override;

protected:
  void paint(QPainter* painter,
             const QStyleOptionGraphicsItem* option,
             QWidget* widget = 0) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
private:

  Connection& _connection;
  ConnectionGeometry& _connectionGeometry;
  ConnectionPainter const& _connectionPainter;
};

#endif //  CONNECTION_GRAPHICS_OBJECT_H
