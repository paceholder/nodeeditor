#pragma once

#include <QtCore/QPointF>
#include <QtGui/QTransform>

class QGraphicsScene;

namespace QtNodes {

class NodeGraphicsObject;

NodeGraphicsObject *locateNodeAt(QPointF scenePoint,
                                 QGraphicsScene &scene,
                                 QTransform const &viewTransform);

} // namespace QtNodes
