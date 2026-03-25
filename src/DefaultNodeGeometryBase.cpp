#include "DefaultNodeGeometryBase.hpp"

#include "AbstractGraphModel.hpp"
#include "ConnectionIdUtils.hpp"
#include "NodeRenderingUtils.hpp"
#include "NodeData.hpp"

#include <optional>

namespace QtNodes {

DefaultNodeGeometryBase::DefaultNodeGeometryBase(AbstractGraphModel &graphModel)
    : AbstractNodeGeometry(graphModel)
    , _portSize(20)
    , _portSpacing(10)
    , _fontMetrics(QFont())
    , _boldFontMetrics(QFont())
{
    QFont f;
    f.setBold(true);
    _boldFontMetrics = QFontMetrics(f);

    _portSize = _fontMetrics.height();
}

QRectF DefaultNodeGeometryBase::boundingRect(NodeId const nodeId) const
{
    QSize s = size(nodeId);
    std::optional<NodeStyle> fallback_style;
    NodeStyle const &style = node_rendering::resolved_node_style(_graphModel, nodeId, fallback_style);
    QMarginsF const margins = node_rendering::node_visual_margins(style.shadowEnabled());

    QRectF r(QPointF(0, 0), s);

    return r.marginsAdded(margins);
}

QSize DefaultNodeGeometryBase::size(NodeId const nodeId) const
{
    return _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);
}

QRectF DefaultNodeGeometryBase::captionRect(NodeId const nodeId) const
{
    if (!_graphModel.nodeData<bool>(nodeId, NodeRole::CaptionVisible))
        return QRect();

    QString name = _graphModel.nodeData<QString>(nodeId, NodeRole::Caption);

    return _boldFontMetrics.boundingRect(name);
}

QRectF DefaultNodeGeometryBase::portTextRect(NodeId const nodeId,
                                             PortType const portType,
                                             PortIndex const portIndex) const
{
    QString s;
    if (_graphModel.portData<bool>(nodeId, portType, portIndex, PortRole::CaptionVisible)) {
        s = _graphModel.portData<QString>(nodeId, portType, portIndex, PortRole::Caption);
    } else {
        auto portData = _graphModel.portData(nodeId, portType, portIndex, PortRole::DataType);

        s = portData.value<NodeDataType>().name;
    }

    return _fontMetrics.boundingRect(s);
}

unsigned int DefaultNodeGeometryBase::maxPortsExtent(NodeId const nodeId) const
{
    PortCount nInPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::InPortCount);

    PortCount nOutPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::OutPortCount);

    unsigned int maxNumOfEntries = std::max(nInPorts, nOutPorts);
    unsigned int step = _portSize + _portSpacing;

    return step * maxNumOfEntries;
}

unsigned int DefaultNodeGeometryBase::maxPortsTextAdvance(NodeId const nodeId,
                                                          PortType const portType) const
{
    unsigned int width = 0;

    size_t const n = _graphModel.nodeData(nodeId, portCountRole(portType)).toUInt();

    for (PortIndex portIndex = 0ul; portIndex < n; ++portIndex) {
        QString name;

        if (_graphModel.portData<bool>(nodeId, portType, portIndex, PortRole::CaptionVisible)) {
            name = _graphModel.portData<QString>(nodeId, portType, portIndex, PortRole::Caption);
        } else {
            NodeDataType portData = _graphModel.portData<NodeDataType>(nodeId,
                                                                       portType,
                                                                       portIndex,
                                                                       PortRole::DataType);

            name = portData.name;
        }

#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        width = std::max(unsigned(_fontMetrics.horizontalAdvance(name)), width);
#else
        width = std::max(unsigned(_fontMetrics.width(name)), width);
#endif
    }

    return width;
}

} // namespace QtNodes
