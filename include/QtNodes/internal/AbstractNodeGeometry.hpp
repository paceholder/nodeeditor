#pragma once

#include "Definitions.hpp"
#include "Export.hpp"

#include <QRectF>
#include <QSize>
#include <QTransform>

namespace QtNodes {

class AbstractGraphModel;

class NODE_EDITOR_PUBLIC AbstractNodeGeometry
{
public:
    AbstractNodeGeometry(AbstractGraphModel &);
    virtual ~AbstractNodeGeometry() {}

    /**
   * The node's size plus some additional margin around it to account for drawing
   * effects (for example shadows) or node's parts outside the size rectangle
   * (for example port points).
   */
    virtual QRectF boundingRect(NodeId const nodeId) const = 0;

    /// A direct rectangle defining the borders of the node's rectangle.
    virtual QSize size(NodeId const nodeId) const = 0;

    /**
   * The function is triggeren when a nuber of ports is changed or when an
   * embedded widget needs an update.
   */
    virtual void recomputeSize(NodeId const nodeId) const = 0;

    /// Port position in node's coordinate system.
    virtual QPointF portPosition(NodeId const nodeId,
                                 PortType const portType,
                                 PortIndex const index) const
        = 0;

    /// A convenience function using the `portPosition` and a given transformation.
    virtual QPointF portScenePosition(NodeId const nodeId,
                                      PortType const portType,
                                      PortIndex const index,
                                      QTransform const &t) const;

    /// Defines where to draw port label. The point corresponds to a font baseline.
    virtual QPointF portTextPosition(NodeId const nodeId,
                                     PortType const portType,
                                     PortIndex const portIndex) const
        = 0;

    /**
   * Defines where to start drawing the caption. The point corresponds to a font
   * baseline.
   */
    virtual QPointF captionPosition(NodeId const nodeId) const = 0;

    /// Caption rect is needed for estimating the total node size.
    virtual QRectF captionRect(NodeId const nodeId) const = 0;

    /// Position for an embedded widget. Return any value if you don't embed.
    virtual QPointF widgetPosition(NodeId const nodeId) const = 0;

    virtual PortIndex checkPortHit(NodeId const nodeId,
                                   PortType const portType,
                                   QPointF const nodePoint) const;

    virtual QRect resizeHandleRect(NodeId const nodeId) const = 0;

protected:
    AbstractGraphModel &_graphModel;
};

} // namespace QtNodes
