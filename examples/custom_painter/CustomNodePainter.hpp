#pragma once

#include <QtNodes/AbstractNodePainter>

#include <QPainter>

namespace QtNodes {
class NodeGraphicsObject;
class AbstractNodeGeometry;
class AbstractGraphModel;
} // namespace QtNodes

using QtNodes::AbstractNodePainter;
using QtNodes::NodeGraphicsObject;

/// Custom node painter that draws nodes with rounded corners and a gradient
class CustomNodePainter : public AbstractNodePainter
{
public:
    void paint(QPainter *painter, NodeGraphicsObject &ngo) const override;

private:
    void drawBackground(QPainter *painter, NodeGraphicsObject &ngo) const;
    void drawCaption(QPainter *painter, NodeGraphicsObject &ngo) const;
    void drawPorts(QPainter *painter, NodeGraphicsObject &ngo) const;
};
