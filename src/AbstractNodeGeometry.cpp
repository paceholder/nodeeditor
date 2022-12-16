#include "AbstractNodeGeometry.hpp"

#include "AbstractGraphModel.hpp"
#include "StyleCollection.hpp"

#include <QMargins>

#include <cmath>

namespace QtNodes {

AbstractNodeGeometry::AbstractNodeGeometry(AbstractGraphModel &graphModel)
    : _graphModel(graphModel)
{
    //
}

QRectF AbstractNodeGeometry::boundingRect(NodeId const nodeId) const
{
    QSize s = size(nodeId);

    double ratio = 0.20;

    int widthMargin = s.width() * ratio;
    int heightMargin = s.height() * ratio;

    QMargins margins(widthMargin, heightMargin, widthMargin, heightMargin);

    QRectF r(QPointF(0, 0), s);

    return r.marginsAdded(margins);
}

QPointF AbstractNodeGeometry::portScenePosition(NodeId const nodeId,
                                                PortType const portType,
                                                PortIndex const index,
                                                QTransform const &t) const
{
    QPointF result = portPosition(nodeId, portType, index);

    return t.map(result);
}

PortIndex AbstractNodeGeometry::checkPortHit(NodeId const nodeId,
                                             PortType const portType,
                                             QPointF const nodePoint) const
{
    auto const &nodeStyle = StyleCollection::nodeStyle();

    PortIndex result = InvalidPortIndex;

    if (portType == PortType::None)
        return result;

    double const tolerance = 2.0 * nodeStyle.ConnectionPointDiameter;

    size_t const n = _graphModel.nodeData<unsigned int>(nodeId,
                                                        (portType == PortType::Out)
                                                            ? NodeRole::OutPortCount
                                                            : NodeRole::InPortCount);

    for (unsigned int portIndex = 0; portIndex < n; ++portIndex) {
        auto pp = portPosition(nodeId, portType, portIndex);

        QPointF p = pp - nodePoint;
        auto distance = std::sqrt(QPointF::dotProduct(p, p));

        if (distance < tolerance) {
            result = portIndex;
            break;
        }
    }

    return result;
}

} // namespace QtNodes
