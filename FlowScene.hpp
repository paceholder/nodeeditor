#ifndef _FLOW_SCENE_HPP_
#define _FLOW_SCENE_HPP_

#include <QtCore/QMap>
#include <QtWidgets/QGraphicsScene>

class FlowScene: public QGraphicsScene
{
public:
  FlowScene() {}

  void
  registerFlowItem(FlowItemIterface* flowItemIterface);

private:
  QMap<QString, FlowItemIterface*> _registeredInterfaces;
};

#endif //  _FLOW_SCENE_HPP_
