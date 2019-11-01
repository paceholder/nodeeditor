#pragma once

#include <QtCore/QUuid>
#include <QtWidgets/QGraphicsObject>

#include "Connection.hpp"

#include "NodeGeometry.hpp"
#include "NodeState.hpp"

class QGraphicsProxyWidget;

namespace QtNodes
{

class FlowScene;
class FlowItemEntry;

/// Class reacts on GUI events, mouse clicks and
/// forwards painting operation.
class GroupGraphicsObject : public QGraphicsObject
{
  Q_OBJECT

public:
  GroupGraphicsObject(){}
};
}