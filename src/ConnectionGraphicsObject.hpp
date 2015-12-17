#ifndef CONNECTION_GRAPHICS_OBJECT_H
#define CONNECTION_GRAPHICS_OBJECT_H

#include <QtWidgets/QGraphicsObject>

class QGraphicsSceneMouseEvent;

class Connection;
class ConnectionGeometry;

class ConnectionGraphicsObject
  : public QGraphicsObject
{
  Q_OBJECT

public:

  ConnectionGraphicsObject(Connection& connection,
                           ConnectionGeometry& connectionGeometry);

public:

  QRectF boundingRect() const override;

public slots:
  void onItemMoved(QUuid id, QPointF const &offset);

  QPainterPath shape() const override;

public:
  ConnectionGeometry& connectionGeometry();

protected:
  void paint(QPainter* painter,
             QStyleOptionGraphicsItem const* option,
             QWidget* widget = 0) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;
private:

  Connection& _connection;
  ConnectionGeometry& _connectionGeometry;
};

#endif //  CONNECTION_GRAPHICS_OBJECT_H
