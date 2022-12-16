#pragma once

#include <QPainter>

#include "Export.hpp"

class QPainter;

namespace QtNodes {

class NodeGraphicsObject;
class NodeDataModel;

/// Class enables custom painting.
class NODE_EDITOR_PUBLIC AbstractNodePainter
{
public:
    virtual ~AbstractNodePainter() = default;

    /**
   * Reimplement this function in order to have a custom painting.
   *
   * Useful functions:
   * `NodeGraphicsObject::nodeScene()->nodeGeometry()`
   * `NodeGraphicsObject::graphModel()`
   */
    virtual void paint(QPainter *painter, NodeGraphicsObject &ngo) const = 0;
};
} // namespace QtNodes
