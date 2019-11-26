#pragma once

#include <QtWidgets/QGraphicsRectItem>

#include "NodeGroup.hpp"

namespace QtNodes
{

class FlowScene;
class NodeGroup;
class NodeGraphicsObject;

class GroupGraphicsObject : public QObject, public QGraphicsRectItem
{
  Q_OBJECT
public:
  GroupGraphicsObject(FlowScene& scene, NodeGroup& nodeGroup);

  virtual ~GroupGraphicsObject();

  NodeGroup& group();

  NodeGroup const& group() const;

  QRectF boundingRect() const override;

  enum { Type = UserType + 3 };

  int type() const override
  {
    return Type;
  }

  void setColor(QColor color);

  void setHoverColor();

  void setFillColor();

  void setLockedColor();

  void resetSize();

  void addObject(NodeGraphicsObject& object);

  QColor _currentColor;

protected:
  void paint(QPainter* painter,
             QStyleOptionGraphicsItem const* option,
             QWidget* widget = 0) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

  const QColor _fillColor = "#20a5b084";
  const QColor _borderColor = "#20aaaaaa";
  const QColor _hoverColor = "#2083a4af";
  const QColor _lockedColor = "20ffffff";

private:
  FlowScene& _scene;

  NodeGroup& _group;

  static constexpr double _roundedBorderRadius = 8.0;
  static constexpr double _groupBorderX = 1.0;
  static constexpr double _groupBorderY = _groupBorderX * 0.5;
  static constexpr QMarginsF  _margins = QMarginsF(_groupBorderX, _groupBorderY, _groupBorderX, _groupBorderY);

  static constexpr double _defaultWidth = 50.0;
  static constexpr double _defaultHeight = 50.0;
};

}  // namespace QtNodes
