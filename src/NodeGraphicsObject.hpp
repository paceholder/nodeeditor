#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"

#include "NodeGeometry.hpp"
#include "NodeState.hpp"

class FlowScene;
class FlowItemEntry;

class QGraphicsProxyWidget;

/// Class reacts on GUI events, mouse clicks and
/// forwards painting operation.
class NodeGraphicsObject : public QGraphicsObject
{
  Q_OBJECT

public:
  NodeGraphicsObject(FlowScene &scene,
                     std::shared_ptr<Node>& node);

  virtual
  ~NodeGraphicsObject();

  std::weak_ptr<Node>&
  node();

  QRectF
  boundingRect() const override;

  void
  setGeometryChanged();

  /// Visits all attached connections and corrects
  /// their corresponding end points.
  void
  moveConnections() const;

protected:
  void
  paint(QPainter*                       painter,
        QStyleOptionGraphicsItem const* option,
        QWidget*                        widget = 0) override;

  QVariant
  itemChange(GraphicsItemChange change, const QVariant &value) override;

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

  void
  hoverMoveEvent(QGraphicsSceneHoverEvent *) override;

private:
  void
  embedQWidget();

private:

  FlowScene & _scene;

  std::weak_ptr<Node> _node;

  // either nullptr or owned by parent QGraphicsItem
  QGraphicsProxyWidget * _proxyWidget;
};
