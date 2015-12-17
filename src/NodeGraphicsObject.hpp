#ifndef NODE_GRAPHICS_OBJECT_HPP
#define NODE_GRAPHICS_OBJECT_HPP

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"

#include "NodeGeometry.hpp"
#include "NodeState.hpp"

class FlowItemEntry;

class NodeGraphicsObject : public QGraphicsObject
{
  Q_OBJECT

public:
  NodeGraphicsObject(Node* node,
                     NodeState& nodeState,
                     NodeGeometry& nodeGeometry);

  void initializeNode();

  QRectF boundingRect() const override;

public:

  QPointF connectionPointScenePosition(std::pair<QUuid, int> address,
                                       EndType endType) const;

  QPointF connectionPointScenePosition(int index,
                                       EndType type) const;

  void reactToPossibleConnection(EndType draggingEnd,
                                 QPointF const &scenePoint);

  bool canConnect(EndType draggingEnd, QPointF const &scenePoint);

  std::pair<QUuid, int>
  connect(Connection const* connection,
          EndType draggingEnd,
          int hit);

  std::pair<QUuid, int>
  connect(Connection const* connection,
          EndType draggingEnd,
          QPointF const& scenePoint);

  void disconnect(Connection const* connection,
                  EndType endType,
                  int hit);

signals:
  void itemMoved(QUuid, QPointF const &);

protected:
  void paint(QPainter*                       painter,
             QStyleOptionGraphicsItem const* option,
             QWidget*                        widget = 0) override;

  void mousePressEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  void embedQWidget();

  int checkHitScenePoint(EndType endType,
                         QPointF const eventPoint) const;

  int checkHitSinkScenePoint(QPointF const eventPoint) const;

  int checkHitSourceScenePoint(QPointF const eventPoint) const;

private:
  // addressing

  Node* _node;


  NodeState& _nodeState;

  // painting

  NodeGeometry& _nodeGeometry;
};

#endif // NODE_GRAPHICS_OBJECT_HPP
