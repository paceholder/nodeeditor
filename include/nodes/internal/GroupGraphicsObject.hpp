#pragma once

#include <QtWidgets/QGraphicsObject>

#include "NodeGroup.hpp"

namespace QtNodes
{

class FlowScene;
class NodeGroup;

class GroupRectItem : public QGraphicsRectItem
{
  friend class GroupGraphicsObject;
public:
  GroupRectItem(QRectF rect, QGraphicsObject * parent = nullptr);


  QRectF
  boundingRect() const override;

  enum { Type = UserType + 4 };

  int
  type() const override
  {
    return Type;
  }

  void
  setHoverColor()
  {
    _currentColor = _hoverColor;
  }

  void
  setFillColor()
  {
    _currentColor = _fillColor;
  }

  QColor _currentColor;

protected:
  void
  paint(QPainter*                       painter,
        QStyleOptionGraphicsItem const* option,
        QWidget*                        widget = 0) override;
private:
  static constexpr double roundedBorderRadius = 8.0;

  const QColor _fillColor = "#50a5b084";
  const QColor _borderColor = "#50aaaaaa";
  const QColor _hoverColor = "#5083a4af";

};

class GroupGraphicsObject : public QGraphicsObject
{
  Q_OBJECT
public:
  GroupGraphicsObject(FlowScene& scene,
                      NodeGroup& nodeGroup);

  virtual
  ~GroupGraphicsObject();

  NodeGroup&
  group();

  NodeGroup const&
  group() const;

  QRectF
  boundingRect() const override;

  enum { Type = UserType + 3 };

  int
  type() const override
  {
    return Type;
  }

protected:
  void
  paint(QPainter*                       painter,
        QStyleOptionGraphicsItem const* option,
        QWidget*                        widget = 0) override;

  void
  hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void
  hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

//  void
//  mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

//  void
//  mousePressEvent(QGraphicsSceneMouseEvent* event) override;

//  void
//  mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;

private:
  FlowScene& _scene;

  NodeGroup& _group;

  GroupRectItem _areaRect;
};

}
