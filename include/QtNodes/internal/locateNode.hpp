#pragma once

#include <QPointF>
#include <QTransform>

class QGraphicsScene;

namespace QtNodes {

class NodeGraphicsObject;

NodeGraphicsObject *locateNodeAt(QPointF scenePoint,
                                 QGraphicsScene &scene,
                                 QTransform const &viewTransform);

} // namespace QtNodes
