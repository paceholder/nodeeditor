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

  void updateBounds();

  void setColor(QColor color)
  {
    _currentColor = color;
  }

  void setHoverColor()
  {
    setColor(_hoverColor);
  }

  void setFillColor()
  {
    setColor(_fillColor);
  }

  void resetSize()
  {
    setRect(x(), y(), _defaultWidth, _defaultHeight);
  }

  void addObject(NodeGraphicsObject& object);

  QColor _currentColor;

protected:
  void paint(QPainter* painter,
             QStyleOptionGraphicsItem const* option,
             QWidget* widget = 0) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

private:
  FlowScene& _scene;

  NodeGroup& _group;

  static constexpr double _roundedBorderRadius = 8.0;
  static constexpr double _groupBorderX = 1.0;
  static constexpr double _groupBorderY = _groupBorderX * 0.5;

  static constexpr double _defaultWidth = 50.0;
  static constexpr double _defaultHeight = 50.0;

  const QColor _fillColor = "#50a5b084";
  const QColor _borderColor = "#50aaaaaa";
  const QColor _hoverColor = "#5083a4af";
};

}  // namespace QtNodes
