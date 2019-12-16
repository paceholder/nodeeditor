#pragma once

#include <QtWidgets/QGraphicsRectItem>
#include <QPen>

#include <memory>

#include "Connection.hpp"
#include "NodeGroup.hpp"

class PadlockGraphicsItem
  : public QGraphicsPixmapItem
{
public:
  PadlockGraphicsItem(QGraphicsItem* parent = nullptr);

  PadlockGraphicsItem(const QPixmap& pixmap, QGraphicsItem* parent = nullptr);

  double scaleFactor() const;

private:

  double _scaleFactor{};

private:
  static constexpr double _iconSize = 24.0;
};

namespace QtNodes
{

class FlowScene;
class NodeGroup;
class NodeGraphicsObject;

class GroupGraphicsObject
  : public QObject
  , public QGraphicsRectItem
{
  Q_OBJECT

public:

  GroupGraphicsObject(FlowScene& scene, NodeGroup& nodeGroup);

  GroupGraphicsObject(const GroupGraphicsObject& ggo) = delete;
  GroupGraphicsObject& operator=(const GroupGraphicsObject& other) = delete;
  GroupGraphicsObject(GroupGraphicsObject&& ggo) = default;
  GroupGraphicsObject& operator=(GroupGraphicsObject&& other) = default;

  ~GroupGraphicsObject() override;

  NodeGroup&
  group();

  NodeGroup const&
  group() const;

  QRectF
  boundingRect() const override;

  enum { Type = UserType + 3 };

  int type() const override
  {
    return Type;
  }

  void
  setFillColor(const QColor& color);

  void
  setBorderColor(const QColor& color);

  void
  resetSize();

  void
  moveConnections();

  void
  moveNodes(const QPointF& offset);

  void
  lock(bool locked);

  bool
  locked() const;

  void
  positionLockedIcon();

  void
  setHovered(bool hovered);

  void
  setPossibleChild(NodeGraphicsObject* possibleChild);

  void
  unsetPossibleChild();

  std::vector<std::shared_ptr<Connection> >
  connections() const;

  void
  setPosition(const QPointF& position);

protected:
  void
  paint(QPainter* painter,
        QStyleOptionGraphicsItem const* option,
        QWidget* widget = nullptr) override;

  void
  hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;

  void
  hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

  void
  mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;

  void
  mouseDoubleClickEvent(QGraphicsSceneMouseEvent* event) override;

public:

  QColor _currentFillColor;

  QColor _currentBorderColor;

  const QColor kUnlockedFillColor  = QColor("#20a5b084");
  const QColor kUnlockedHoverColor = QColor("#2083a4af");

  const QColor kLockedFillColor  = QColor("#3fe0bebc");
  const QColor kLockedHoverColor = QColor("#3feecdcb");

  const QColor kSelectedBorderColor   = QColor("#eeffa500");
  const QColor kUnselectedBorderColor = QColor("#eeaaaaaa");

  QPen _borderPen;

private:
  FlowScene& _scene;

  NodeGroup& _group;

  PadlockGraphicsItem* _lockedGraphicsItem;
  PadlockGraphicsItem* _unlockedGraphicsItem;

  QPixmap _lockedIcon{"://padlock-lock.png"};
  QPixmap _unlockedIcon{"://padlock-unlock.png"};

  NodeGraphicsObject* _possibleChild;

  bool _locked;

  static constexpr double _groupBorderX = 25.0;
  static constexpr double _groupBorderY = _groupBorderX * 0.8;
  static constexpr double _roundedBorderRadius = _groupBorderY;
  static constexpr QMarginsF  _margins = QMarginsF(_groupBorderX, _groupBorderY, _groupBorderX, _groupBorderY);

  static constexpr double _defaultWidth = 50.0;
  static constexpr double _defaultHeight = 50.0;

  static constexpr double _groupAreaZValue = 2.0;
};

}  // namespace QtNodes
