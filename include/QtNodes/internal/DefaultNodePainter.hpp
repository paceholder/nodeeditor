#pragma once

#include <QtGui/QPainter>

#include "AbstractNodePainter.hpp"
#include "Definitions.hpp"

namespace QtNodes {

class BasicGraphicsScene;
class GraphModel;
class NodeGeometry;
class NodeGraphicsObject;
class NodeState;

/// @ Lightweight class incapsulating paint code.
class NODE_EDITOR_PUBLIC DefaultNodePainter : public AbstractNodePainter
{
public:
    void paint(QPainter *painter, NodeGraphicsObject &ngo) const override;

    void drawNodeRect(QPainter *painter, NodeGraphicsObject &ngo) const;

    void drawConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const;

    void drawFilledConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const;

    void drawNodeCaption(QPainter *painter, NodeGraphicsObject &ngo) const;

    void drawEntryLabels(QPainter *painter, NodeGraphicsObject &ngo) const;

    void drawResizeRect(QPainter *painter, NodeGraphicsObject &ngo) const;
};
} // namespace QtNodes
