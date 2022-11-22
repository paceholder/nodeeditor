#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>


#include "NodeGeometry.hpp"
#include "NodeState.hpp"
#include "PortType.hpp"
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <array>
class QGraphicsProxyWidget;

namespace QtNodes
{

class FlowScene;
class FlowItemEntry;
class Group;
class Connection;

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

  bool 
  isCollapsed() {return collapsed;}

  void
  setGeometryChanged();

  /// Visits all attached connections and corrects
  /// their corresponding end points.
  void
  moveConnections() const;

  enum { Type = UserType + 5 };

  int
  type() const override { return Type; }

  void
  lock(bool locked);

  uint8_t r, g, b;
  QLineEdit* nameLineEdit;
  QGraphicsProxyWidget* _proxyWidget;
  QGraphicsProxyWidget* _collapseButton;
  QPushButton *collapseButtonWidget;

  int getSizeX() {return sizeX;}
  int getSizeY() {return sizeY;}
  int getSavedSizeX() {return savedSizeX;}
  int getSavedSizeY() {return savedSizeY;}

  void setSizeX(int size) {sizeX = size;}
  void setSizeY(int size) {sizeY = size;}

  void Collapse();
  
  QPointF portScenePosition(int i, PortType type) const;
  
  QPointF portPosition(int i, PortType type) const;

  int
    checkHitScenePoint(PortType portType,
                      QPointF const point,
                      QTransform t = QTransform()) const;


  std::array<std::vector<Node*>, 3> inOutNodes;
  std::array<std::vector<int>, 3> inOutPorts;
  
  //Store pointers to connections from in and out.
  std::array<std::vector<Connection*>, 3> inOutConnections;
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
  QPointF oldPosition;
  FlowScene & _scene;
  Group& _group;

  bool isResizingX=false;
  bool isResizingY=false;
  bool isResizingXY=false;

  int sizeX=1000;  
  int sizeY=1000; 
  

  float inputSize = 10;
  float spacing = 40;
  float topPadding = 30;
  bool collapsed=false;
  int savedSizeX, savedSizeY;
  
  //Store connections that stay strictly inside group's bound, to set them invisible when collapsing
  std::vector<Connection*> unusedConnections;

  //Store the labels of the in and out when collapsed
  std::array<std::vector<QString>, 3> inOutLabels;
  

  
signals:
  void CollapseTriggered(bool state);

};
}
