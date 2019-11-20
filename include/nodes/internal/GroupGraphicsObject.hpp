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
  GroupRectItem(QGraphicsObject * parent = nullptr,
                QRectF rect = QRectF(0, 0, _defaultWidth, _defaultHeight));


  QRectF
  boundingRect() const override;

  enum { Type = UserType + 4 };

  int
  type() const override
  {
    return Type;
  }

  void setColor(QColor color)
  {
    _currentColor = color;
  }

  void
  setHoverColor()
  {
    setColor(_hoverColor);
  }

  void
  setFillColor()
  {
    setColor(_fillColor);
  }

  void resetSize()
  {
    setRect(x(), y(), _defaultWidth, _defaultHeight);
  }

  QColor _currentColor;

protected:
  void
  paint(QPainter*                       painter,
        QStyleOptionGraphicsItem const* option,
        QWidget*                        widget = 0) override;

private:
  static constexpr double roundedBorderRadius = 8.0;

  static constexpr double _defaultWidth  = 50.0;
  static constexpr double _defaultHeight = 50.0;

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

  void
  updateBounds();

//  QPointF centerInSceneCoords() const;

//  QPointF topLeftInSceneCoords() const;

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
