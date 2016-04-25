#pragma once

#include <memory>

#include <QtCore/QUuid>

#include <QtWidgets/QGraphicsObject>

class QGraphicsSceneMouseEvent;

class Connection;
class ConnectionGeometry;

/// Graphic Object for connection. Adds itself to scene
class ConnectionGraphicsObject
  : public QGraphicsObject
{
  Q_OBJECT

public:

  ConnectionGraphicsObject(Connection &connection);

public:

  QRectF boundingRect() const override;

//signals:

  //void connectionDraggedTo(PortType postType, QPointF const &scenePoint);

public slots:

  void onItemMoved(QUuid id, QPointF const &offset);

  QPainterPath shape() const override;

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

  void addGraphicsEffect();

private:

  Connection & _connection;
};
