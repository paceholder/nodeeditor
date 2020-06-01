#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"

#include "NodeDataModel.hpp"
#include "NodeGeometry.hpp"
#include "NodeState.hpp"

class QGraphicsProxyWidget;

namespace QtNodes
{

class ThreadStatusIcon : public QGraphicsPixmapItem
{
public:
  ThreadStatusIcon(int iconSize,
                   QGraphicsItem* parent = nullptr);

  void
  setStatus(const NodeThreadState state);

  void
  setIconSize(int iconSize);

  void
  updateScaleFactor();

private:
  int _iconSize;

  const QPixmap _statusUpdated{"://status_icons/updated.png"};
  const QPixmap _statusProcessing{"://status_icons/processing.png"};
  const QPixmap _statusPending{"://status_icons/pending.png"};
  const QPixmap _statusInvalid{"://status_icons/invalid.png"};
};

class FlowScene;
class FlowItemEntry;
class GroupGraphicsObject;

/**
 * @brief The NodeGraphicsObject class handles the graphical part of a node.
 * Each node is associated with a unique NodeGraphicsObject. This class reacts
 * on GUI events, mouse clicks and forwards painting operation.
 */
class NodeGraphicsObject : public QGraphicsObject
{
  Q_OBJECT

public:
  NodeGraphicsObject(FlowScene &scene,
                     Node& node);

  virtual
  ~NodeGraphicsObject();

  Node&
  node();

  Node const&
  node() const;

  QRectF
  boundingRect() const override;

  void
  setGeometryChanged();

  /// Visits all attached connections and corrects
  /// their corresponding end points.
  void
  moveConnections() const;

  enum { Type = UserType + 1 };

  int
  type() const override
  {
    return Type;
  }

  void
  lock(bool locked);

  /**
   * @brief Method that updates the graphical object accounting for changes in
   * the node's geometry and ports.
   */
  void
  updateGeometry();

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

  void
  mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

  void
  contextMenuEvent(QGraphicsSceneContextMenuEvent* event) override;

private:
  void
  embedQWidget();

private:

  FlowScene & _scene;

  Node& _node;

  bool _locked;

  bool _draggingIntoGroup;

  GroupGraphicsObject* _possibleGroup;

  QRectF _originalGroupSize;

  std::unique_ptr<ThreadStatusIcon> _statusIcon;

  // either nullptr or owned by parent QGraphicsItem
  QGraphicsProxyWidget * _proxyWidget;
};
}
