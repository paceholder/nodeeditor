#include "DefaultVerticalNodeGeometry.hpp"

#include "AbstractGraphModel.hpp"
#include "NodeData.hpp"

#include <QPoint>
#include <QRect>
#include <QWidget>

namespace QtNodes {

DefaultVerticalNodeGeometry::DefaultVerticalNodeGeometry(AbstractGraphModel &graphModel)
    : AbstractNodeGeometry(graphModel)
    , _portSize(20)
    , _portSpasing(10)
    , _fontMetrics(QFont())
    , _boldFontMetrics(QFont())
{
    QFont f;
    f.setBold(true);
    _boldFontMetrics = QFontMetrics(f);

    _portSize = _fontMetrics.height();
}

QRectF DefaultVerticalNodeGeometry::boundingRect(NodeId const nodeId) const
{
    QSize s = size(nodeId);

    qreal marginSize = 2.0 * _portSpasing;
    QMargins margins(marginSize, marginSize, marginSize, marginSize);

    QRectF r(QPointF(0, 0), s);

    return r.marginsAdded(margins);
}

QSize DefaultVerticalNodeGeometry::size(NodeId const nodeId) const
{
    return _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);
}

void DefaultVerticalNodeGeometry::recomputeSize(NodeId const nodeId) const
{
    unsigned int height = _portSpasing; // maxHorizontalPortsExtent(nodeId);

    if (auto w = _graphModel.nodeData<QWidget *>(nodeId, NodeRole::Widget)) {
        height = std::max(height, static_cast<unsigned int>(w->height()));
    }

    QRectF const capRect = captionRect(nodeId);

    height += capRect.height();

    height += _portSpasing;
    height += _portSpasing;

    PortCount nInPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::InPortCount);
    PortCount nOutPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::OutPortCount);

    // Adding double step (top and bottom) to reserve space for port captions.

    height += portCaptionsHeight(nodeId, PortType::In);
    height += portCaptionsHeight(nodeId, PortType::Out);

    unsigned int inPortWidth = maxPortsTextAdvance(nodeId, PortType::In);
    unsigned int outPortWidth = maxPortsTextAdvance(nodeId, PortType::Out);

    unsigned int totalInPortsWidth = nInPorts > 0
                                         ? inPortWidth * nInPorts + _portSpasing * (nInPorts - 1)
                                         : 0;

    unsigned int totalOutPortsWidth = nOutPorts > 0 ? outPortWidth * nOutPorts
                                                          + _portSpasing * (nOutPorts - 1)
                                                    : 0;

    unsigned int width = std::max(totalInPortsWidth, totalOutPortsWidth);

    if (auto w = _graphModel.nodeData<QWidget *>(nodeId, NodeRole::Widget)) {
        width = std::max(width, static_cast<unsigned int>(w->width()));
    }

    width = std::max(width, static_cast<unsigned int>(capRect.width()));

    width += _portSpasing;
    width += _portSpasing;

    QSize size(width, height);

    _graphModel.setNodeData(nodeId, NodeRole::Size, size);
}

QPointF DefaultVerticalNodeGeometry::portPosition(NodeId const nodeId,
                                                  PortType const portType,
                                                  PortIndex const portIndex) const
{
    QPointF result;

    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);

    switch (portType) {
    case PortType::In: {
        unsigned int inPortWidth = maxPortsTextAdvance(nodeId, PortType::In) + _portSpasing;

        PortCount nInPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::InPortCount);

        double x = (size.width() - (nInPorts - 1) * inPortWidth) / 2.0 + portIndex * inPortWidth;

        double y = 0.0;

        result = QPointF(x, y);

        break;
    }

    case PortType::Out: {
        unsigned int outPortWidth = maxPortsTextAdvance(nodeId, PortType::Out) + _portSpasing;
        PortCount nOutPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::OutPortCount);

        double x = (size.width() - (nOutPorts - 1) * outPortWidth) / 2.0 + portIndex * outPortWidth;

        double y = size.height();

        result = QPointF(x, y);

        break;
    }

    default:
        break;
    }

    return result;
}

QPointF DefaultVerticalNodeGeometry::portTextPosition(NodeId const nodeId,
                                                      PortType const portType,
                                                      PortIndex const portIndex) const
{
    QPointF p = portPosition(nodeId, portType, portIndex);

    QRectF rect = portTextRect(nodeId, portType, portIndex);

    p.setX(p.x() - rect.width() / 2.0);

    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);

    switch (portType) {
    case PortType::In:
        p.setY(5.0 + rect.height());
        break;

    case PortType::Out:
        p.setY(size.height() - 5.0);
        break;

    default:
        break;
    }

    return p;
}

QRectF DefaultVerticalNodeGeometry::captionRect(NodeId const nodeId) const
{
    if (!_graphModel.nodeData<bool>(nodeId, NodeRole::CaptionVisible))
        return QRect();

    QString name = _graphModel.nodeData<QString>(nodeId, NodeRole::Caption);

    return _boldFontMetrics.boundingRect(name);
}

QPointF DefaultVerticalNodeGeometry::captionPosition(NodeId const nodeId) const
{
    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);

    unsigned int step = portCaptionsHeight(nodeId, PortType::In);
    step += _portSpasing;

    auto rect = captionRect(nodeId);

    return QPointF(0.5 * (size.width() - rect.width()), step + rect.height());
}

QPointF DefaultVerticalNodeGeometry::widgetPosition(NodeId const nodeId) const
{
    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);

    unsigned int captionHeight = captionRect(nodeId).height();

    if (auto w = _graphModel.nodeData<QWidget *>(nodeId, NodeRole::Widget)) {
        // If the widget wants to use as much vertical space as possible,
        // place it immediately after the caption.
        if (w->sizePolicy().verticalPolicy() & QSizePolicy::ExpandFlag) {
            return QPointF(_portSpasing + maxPortsTextAdvance(nodeId, PortType::In), captionHeight);
        } else {
            return QPointF(_portSpasing + maxPortsTextAdvance(nodeId, PortType::In),
                           (captionHeight + size.height() - w->height()) / 2.0);
        }
    }
    return QPointF();
}

QRect DefaultVerticalNodeGeometry::resizeHandleRect(NodeId const nodeId) const
{
    QSize size = _graphModel.nodeData<QSize>(nodeId, NodeRole::Size);

    unsigned int rectSize = 7;

    return QRect(size.width() - rectSize, size.height() - rectSize, rectSize, rectSize);
}

QRectF DefaultVerticalNodeGeometry::portTextRect(NodeId const nodeId,
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

unsigned int DefaultVerticalNodeGeometry::maxHorizontalPortsExtent(NodeId const nodeId) const
{
    PortCount nInPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::InPortCount);

    PortCount nOutPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::OutPortCount);

    unsigned int maxNumOfEntries = std::max(nInPorts, nOutPorts);
    unsigned int step = _portSize + _portSpasing;

    return step * maxNumOfEntries;
}

unsigned int DefaultVerticalNodeGeometry::maxPortsTextAdvance(NodeId const nodeId,
                                                              PortType const portType) const
{
    unsigned int width = 0;

    size_t const n = _graphModel
                         .nodeData(nodeId,
                                   (portType == PortType::Out) ? NodeRole::OutPortCount
                                                               : NodeRole::InPortCount)
                         .toUInt();

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

unsigned int DefaultVerticalNodeGeometry::portCaptionsHeight(NodeId const nodeId,
                                                             PortType const portType) const
{
    unsigned int h = 0;

    switch (portType) {
    case PortType::In: {
        PortCount nInPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::InPortCount);
        for (PortIndex i = 0; i < nInPorts; ++i) {
            if (_graphModel.portData<bool>(nodeId, PortType::In, i, PortRole::CaptionVisible)) {
                h += _portSpasing;
                break;
            }
        }
        break;
    }

    case PortType::Out: {
        PortCount nOutPorts = _graphModel.nodeData<PortCount>(nodeId, NodeRole::OutPortCount);
        for (PortIndex i = 0; i < nOutPorts; ++i) {
            if (_graphModel.portData<bool>(nodeId, PortType::Out, i, PortRole::CaptionVisible)) {
                h += _portSpasing;
                break;
            }
        }
        break;
    }

    default:
        break;
    }

    return h;
}

} // namespace QtNodes
