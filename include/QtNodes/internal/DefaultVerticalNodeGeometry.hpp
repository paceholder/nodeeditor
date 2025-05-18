#pragma once

#include "AbstractNodeGeometry.hpp"

#include <QtGui/QFontMetrics>

namespace QtNodes {

class AbstractGraphModel;
class BasicGraphicsScene;

class NODE_EDITOR_PUBLIC DefaultVerticalNodeGeometry : public AbstractNodeGeometry
{
public:
    DefaultVerticalNodeGeometry(AbstractGraphModel &graphModel);

public:
    QRectF boundingRect(NodeId const nodeId) const override;

    QSize size(NodeId const nodeId) const override;

    void recomputeSize(NodeId const nodeId) const override;

    QPointF portPosition(NodeId const nodeId,
                         PortType const portType,
                         PortIndex const index) const override;

    QPointF portTextPosition(NodeId const nodeId,
                             PortType const portType,
                             PortIndex const PortIndex) const override;

    QPointF captionPosition(NodeId const nodeId) const override;

    QRectF captionRect(NodeId const nodeId) const override;

    QPointF widgetPosition(NodeId const nodeId) const override;

    QRect resizeHandleRect(NodeId const nodeId) const override;

private:
    QRectF portTextRect(NodeId const nodeId,
                        PortType const portType,
                        PortIndex const portIndex) const;
    /// Finds
    unsigned int maxHorizontalPortsExtent(NodeId const nodeId) const;

    unsigned int maxPortsTextAdvance(NodeId const nodeId, PortType const portType) const;

    unsigned int portCaptionsHeight(NodeId const nodeId, PortType const portType) const;

private:
    // Some variables are mutable because we need to change drawing
    // metrics corresponding to fontMetrics but this doesn't change
    // constness of the Node.

    mutable unsigned int _portSize;
    unsigned int _portSpasing;
    mutable QFontMetrics _fontMetrics;
    mutable QFontMetrics _boldFontMetrics;
};

} // namespace QtNodes
