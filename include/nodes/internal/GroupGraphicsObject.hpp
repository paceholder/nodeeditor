#pragma once

#include <QtWidgets/QGraphicsObject>

#include "NodeGroup.hpp"

namespace QtNodes
{

class FlowScene;
class NodeGroup;

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

  QRectF _areaRect;
};

}
