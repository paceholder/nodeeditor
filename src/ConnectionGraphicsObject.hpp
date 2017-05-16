#pragma once

#include <memory>

#include <QtCore/QUuid>

#include <QtWidgets/QGraphicsObject>

class QGraphicsSceneMouseEvent;

namespace QtNodes
{

class FlowScene;
class Connection;
class ConnectionGeometry;
class Node;

/// Graphic Object for connection. Adds itself to scene
class ConnectionGraphicsObject
  : public QGraphicsObject
{
  Q_OBJECT

public:

  ConnectionGraphicsObject(FlowScene &scene,
                           Connection &connection);

  virtual
  ~ConnectionGraphicsObject();

  enum { Type = UserType + 2 };
  int
  type() const override { return Type; }

public:

  Connection&
  connection();

  QRectF
  boundingRect() const override;

  QPainterPath
  shape() const override;

  void
  setGeometryChanged();

  /// Updates the position of both ends
  void
  move();

  void
  lock(bool locked);

protected:

  void
  paint(QPainter* painter,
        QStyleOptionGraphicsItem const* option,
        QWidget* widget = 0) override;

  void
  mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void
  mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void
  mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

  void
  hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void
  hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:

  void
  addGraphicsEffect();

private:

  FlowScene & _scene;

  Connection& _connection;
};
}
