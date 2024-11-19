#pragma once

#include "AbstractNodeGeometry.hpp"
#include "Definitions.hpp"
#include <QFont>
#include <QFontMetrics>
#include <QPointF>
#include <QRect>
#include <QRectF>
#include <QSize>

namespace QtNodes {

class DefaultHorizontalNodeGeometry : public AbstractNodeGeometry
{
public:
    DefaultHorizontalNodeGeometry(AbstractGraphModel &graphModel);

public:
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

    /// Finds max number of ports and multiplies by (a port height + interval)
    unsigned int maxVerticalPortsExtent(NodeId const nodeId) const;

    unsigned int maxPortsTextAdvance(NodeId const nodeId, PortType const portType) const;

private:
    static constexpr unsigned int _portSpasing = 10;

    QFontMetrics _fontMetrics{QFont()};
    unsigned int _portSize;
    QFontMetrics _boldFontMetrics{QFont()};
};

} // namespace QtNodes
