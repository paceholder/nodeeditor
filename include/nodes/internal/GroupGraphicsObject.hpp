#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"

#include "NodeGeometry.hpp"
#include "NodeState.hpp"
#include <QtWidgets/QLineEdit>

class QGraphicsProxyWidget;

namespace QtNodes
{

class FlowScene;
class FlowItemEntry;
class Group;

/// Class reacts on GUI events, mouse clicks and
/// forwards painting operation.
class GroupGraphicsObject : public QGraphicsObject
{
  Q_OBJECT

public:
  GroupGraphicsObject(FlowScene &scene, Group& group);

  virtual
  ~GroupGraphicsObject();

  Group&
  group();

  Group const&
  group() const;


  QRectF
  boundingRect() const override;

  void
  setGeometryChanged();

  /// Visits all attached connections and corrects
  /// their corresponding end points.
  void
  moveConnections() const;

  enum { Type = UserType + 3 };

  int
  type() const override { return Type; }

  void
  lock(bool locked);

  uint8_t r, g, b;
  QLineEdit* nameLineEdit;
  QGraphicsProxyWidget* _proxyWidget;

  int getSizeX() {return sizeX;}
  int getSizeY() {return sizeY;}

  void setSizeX(int size) {sizeX = size;}
  void setSizeY(int size) {sizeY = size;}

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
  FlowScene & _scene;
  Group& _group;

  bool isResizingX=false;
  bool isResizingY=false;
  bool isResizingXY=false;

  int sizeX=1000;  
  int sizeY=1000;  

};
}
