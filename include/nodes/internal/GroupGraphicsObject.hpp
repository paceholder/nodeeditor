#pragma once

#include <QtWidgets/QGraphicsRectItem>

#include <memory>

#include "NodeGroup.hpp"
#include "Connection.hpp"

class PadlockGraphicsItem : public QGraphicsPixmapItem
{
public:
  PadlockGraphicsItem(QGraphicsItem* parent = nullptr);
  PadlockGraphicsItem(const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

protected:
  QRectF boundingRect() const override;
};

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

  virtual ~GroupGraphicsObject() override;

  NodeGroup& group();

  NodeGroup const& group() const;

  QRectF boundingRect() const override;

  enum { Type = UserType + 3 };

  int type() const override
  {
    return Type;
  }

  void setColor(const QColor& color);

  void resetSize();

  void moveConnections();

  void moveNodes(const QPointF& offset);

  void lock(bool locked);

  bool locked() const;

  QColor _currentColor;

  void positionLockedIcon();

  void setHovered(bool hovered);

  void setPossibleChild(NodeGraphicsObject* possibleChild);

  void unsetPossibleChild();

  std::vector<std::shared_ptr<Connection>> connections() const;

protected:
  void paint(QPainter* painter,
             QStyleOptionGraphicsItem const* option,
             QWidget* widget = 0) override;

  void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

public:
  const QColor kUnlockedFillColor  = QColor("#20a5b084");
  const QColor kUnlockedHoverColor = QColor("#2083a4af");

  const QColor kLockedFillColor  = QColor("#3fe0bebc");
  const QColor kLockedHoverColor = QColor("#3feecdcb");

  const QColor kBorderColor = QColor("#20aaaaaa");

private:
  FlowScene& _scene;

  NodeGroup& _group;

  QGraphicsPixmapItem* _lockedGraphicsItem;
  QGraphicsPixmapItem* _unlockedGraphicsItem;

  QPixmap _lockedIcon{"://padlock-lock.png"};
  QPixmap _unlockedIcon{"://padlock-unlock.png"};

  NodeGraphicsObject* _possibleChild;

  bool _locked;

  static constexpr double _roundedBorderRadius = 8.0;
  static constexpr double _groupBorderX = 10.0;
  static constexpr double _groupBorderY = _groupBorderX * 0.5;
  static constexpr QMarginsF  _margins = QMarginsF(_groupBorderX, _groupBorderY, _groupBorderX, _groupBorderY);

  static constexpr double _defaultWidth = 50.0;
  static constexpr double _defaultHeight = 50.0;
};

}  // namespace QtNodes
