#pragma once

#include <QtWidgets/QGraphicsObject>

#include "NodeGroup.hpp"

namespace QtNodes
{

class FlowScene;
class NodeGroup;

class GroupRectItem : public QGraphicsRectItem
{
public:
  GroupRectItem(QRectF rect, QGraphicsObject * parent = nullptr);

  void
  paint(QPainter*                       painter,
        QStyleOptionGraphicsItem const* option,
        QWidget*                        widget = 0) override;

  QRectF
  boundingRect() const override;

  enum { Type = UserType + 4 };

  int
  type() const override
  {
    return Type;
  }

protected:
  void
  hoverEnterEvent(QGraphicsSceneHoverEvent*) override;

private:
  static constexpr double roundedBorderRadius = 8.0;

  const QColor _fillColor = "#10a5b084";
  const QColor _borderColor = "#10aaaaaa";
  const QColor _hoverColor = "1083a4af";

  QColor _currentColor;
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

private:
  FlowScene& _scene;

  NodeGroup& _group;

  GroupRectItem _areaRect;
};

}
