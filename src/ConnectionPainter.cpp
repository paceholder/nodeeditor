#include "ConnectionPainter.hpp"

#include <QtGui/QIcon>

#include "AbstractGraphModel.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "Definitions.hpp"
#include "NodeData.hpp"
#include "StyleCollection.hpp"

namespace QtNodes {

static QPainterPath cubicPath(ConnectionGraphicsObject const &connection)
{
    QPointF const &in = connection.endPoint(PortType::In);
    QPointF const &out = connection.endPoint(PortType::Out);

    auto const c1c2 = connection.pointsC1C2();

    // cubic spline
    QPainterPath cubic(out);

    cubic.cubicTo(c1c2.first, c1c2.second, in);

    return cubic;
}

QPainterPath ConnectionPainter::getPainterStroke(ConnectionGraphicsObject const &connection)
{
    auto cubic = cubicPath(connection);

    QPointF const &out = connection.endPoint(PortType::Out);
    QPainterPath result(out);

    unsigned segments = 20;

    for (auto i = 0ul; i < segments; ++i) {
        double ratio = double(i + 1) / segments;
        result.lineTo(cubic.pointAtPercent(ratio));
    }

    QPainterPathStroker stroker;
    stroker.setWidth(10.0);

    return stroker.createStroke(result);
}

#ifdef NODE_DEBUG_DRAWING
static void debugDrawing(QPainter *painter, ConnectionGraphicsObject const &cgo)
{
    Q_UNUSED(painter);

    {
        QPointF const &in = cgo.endPoint(PortType::In);
        QPointF const &out = cgo.endPoint(PortType::Out);

        auto const points = cgo.pointsC1C2();

        painter->setPen(Qt::red);
        painter->setBrush(Qt::red);

        painter->drawLine(QLineF(out, points.first));
        painter->drawLine(QLineF(points.first, points.second));
        painter->drawLine(QLineF(points.second, in));
        painter->drawEllipse(points.first, 3, 3);
        painter->drawEllipse(points.second, 3, 3);

        painter->setBrush(Qt::NoBrush);
        painter->drawPath(cubicPath(cgo));
    }

    {
        painter->setPen(Qt::yellow);
        painter->drawRect(cgo.boundingRect());
    }
}

#endif

static void drawSketchLine(QPainter *painter, ConnectionGraphicsObject const &cgo)
{
    ConnectionState const &state = cgo.connectionState();

    if (state.requiresPort()) {
        auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

        QPen pen;
        pen.setWidth(connectionStyle.constructionLineWidth());
        pen.setColor(connectionStyle.constructionColor());
        pen.setStyle(Qt::DashLine);

        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        auto cubic = cubicPath(cgo);

        // cubic spline
        painter->drawPath(cubic);
    }
}

static void drawHoveredOrSelected(QPainter *painter, ConnectionGraphicsObject const &cgo)
{
    bool const hovered = cgo.connectionState().hovered();
    bool const selected = cgo.isSelected();

    // drawn as a fat background
    if (hovered || selected) {
        auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

        double const lineWidth = connectionStyle.lineWidth();

        QPen pen;
        pen.setWidth(2 * lineWidth);
        pen.setColor(selected ? connectionStyle.selectedHaloColor()
                              : connectionStyle.hoveredColor());

        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        // cubic spline
        auto const cubic = cubicPath(cgo);
        painter->drawPath(cubic);
    }
}

static void drawNormalLine(QPainter *painter, ConnectionGraphicsObject const &cgo)
{
    ConnectionState const &state = cgo.connectionState();

    if (state.requiresPort())
        return;

    // colors

    auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

    QColor normalColorOut = connectionStyle.normalColor();
    QColor normalColorIn = connectionStyle.normalColor();
    QColor selectedColor = connectionStyle.selectedColor();

    bool useGradientColor = false;

    AbstractGraphModel const &graphModel = cgo.graphModel();

    if (connectionStyle.useDataDefinedColors()) {
        using QtNodes::PortType;

        auto const cId = cgo.connectionId();

        auto dataTypeOut = graphModel
                               .portData(cId.outNodeId,
                                         PortType::Out,
                                         cId.outPortIndex,
                                         PortRole::DataType)
                               .value<NodeDataType>();

        auto dataTypeIn
            = graphModel.portData(cId.inNodeId, PortType::In, cId.inPortIndex, PortRole::DataType)
                  .value<NodeDataType>();

        useGradientColor = (dataTypeOut.id != dataTypeIn.id);

        normalColorOut = connectionStyle.normalColor(dataTypeOut.id);
        normalColorIn = connectionStyle.normalColor(dataTypeIn.id);
        selectedColor = normalColorOut.darker(200);
    }

    // geometry

    double const lineWidth = connectionStyle.lineWidth();

    // draw normal line
    QPen p;

    p.setWidth(lineWidth);

    bool const selected = cgo.isSelected();

    auto cubic = cubicPath(cgo);
    if (useGradientColor) {
        painter->setBrush(Qt::NoBrush);

        QColor cOut = normalColorOut;
        if (selected)
            cOut = cOut.darker(200);
        p.setColor(cOut);
        painter->setPen(p);

        unsigned int const segments = 60;

        for (unsigned int i = 0ul; i < segments; ++i) {
            double ratioPrev = double(i) / segments;
            double ratio = double(i + 1) / segments;

            if (i == segments / 2) {
                QColor cIn = normalColorIn;
                if (selected)
                    cIn = cIn.darker(200);

                p.setColor(cIn);
                painter->setPen(p);
            }
            painter->drawLine(cubic.pointAtPercent(ratioPrev), cubic.pointAtPercent(ratio));
        }

        {
            QIcon icon(":convert.png");

            QPixmap pixmap = icon.pixmap(QSize(22, 22));
            painter->drawPixmap(cubic.pointAtPercent(0.50)
                                    - QPoint(pixmap.width() / 2, pixmap.height() / 2),
                                pixmap);
        }
    } else {
        p.setColor(normalColorOut);

        if (selected) {
            p.setColor(selectedColor);
        }

        painter->setPen(p);
        painter->setBrush(Qt::NoBrush);

        painter->drawPath(cubic);
    }
}

void ConnectionPainter::paint(QPainter *painter, ConnectionGraphicsObject const &cgo)
{
    drawHoveredOrSelected(painter, cgo);

    drawSketchLine(painter, cgo);

    drawNormalLine(painter, cgo);

#ifdef NODE_DEBUG_DRAWING
    debugDrawing(painter, cgo);
#endif

    // draw end points
    auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

    double const pointDiameter = connectionStyle.pointDiameter();

    painter->setPen(connectionStyle.constructionColor());
    painter->setBrush(connectionStyle.constructionColor());
    double const pointRadius = pointDiameter / 2.0;
    painter->drawEllipse(cgo.out(), pointRadius, pointRadius);
    painter->drawEllipse(cgo.in(), pointRadius, pointRadius);
}

} // namespace QtNodes
