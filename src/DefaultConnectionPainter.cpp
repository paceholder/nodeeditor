#include "DefaultConnectionPainter.hpp"

#include <QtGui/QIcon>

#include "AbstractGraphModel.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionState.hpp"
#include "Definitions.hpp"
#include "NodeData.hpp"
#include "StyleCollection.hpp"

namespace QtNodes {

QPainterPath DefaultConnectionPainter::cubicPath(ConnectionGraphicsObject const &connection) const
{
    QPointF const &in = connection.endPoint(PortType::In);
    QPointF const &out = connection.endPoint(PortType::Out);

    auto const c1c2 = connection.pointsC1C2();

    // cubic spline
    QPainterPath cubic(out);

    cubic.cubicTo(c1c2.first, c1c2.second, in);

    return cubic;
}

void DefaultConnectionPainter::drawSketchLine(QPainter *painter, ConnectionGraphicsObject const &cgo, QPainterPath const &cubic) const
{
    ConnectionState const &state = cgo.connectionState();

    if (state.requiresPort()) {
        auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

        QPen pen;
        pen.setWidth(static_cast<int>(connectionStyle.constructionLineWidth()));
        pen.setColor(connectionStyle.constructionColor());
        pen.setStyle(Qt::DashLine);

        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        // cubic spline
        painter->drawPath(cubic);
    }
}

void DefaultConnectionPainter::drawHoveredOrSelected(QPainter *painter, ConnectionGraphicsObject const &cgo,  QPainterPath const &cubic) const
{
    bool const hovered = cgo.connectionState().hovered();
    bool const selected = cgo.isSelected();

    // drawn as a fat background
    if (hovered || selected) {
        auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

        double const lineWidth = connectionStyle.lineWidth();

        QPen pen;
        pen.setWidth(static_cast<int>(2 * lineWidth));
        pen.setColor(selected ? connectionStyle.selectedHaloColor()
                              : connectionStyle.hoveredColor());

        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        // cubic spline
        painter->drawPath(cubic);
    }
}

void DefaultConnectionPainter::drawNormalLine(QPainter *painter, ConnectionGraphicsObject const &cgo, QPainterPath const &cubic) const
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

    if (useGradientColor) {
        painter->setBrush(Qt::NoBrush);

        QColor cOut = normalColorOut;
        if (selected)
            cOut = cOut.darker(200);
        p.setColor(cOut);
        painter->setPen(p);

        unsigned int constexpr segments = 60;

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

void DefaultConnectionPainter::paint(QPainter *painter, ConnectionGraphicsObject const &cgo) const
{
    auto cubic = cubicPath(cgo);
    drawHoveredOrSelected(painter, cgo,cubic);

    drawSketchLine(painter, cgo,cubic);

    drawNormalLine(painter, cgo,cubic);

#ifdef NODE_DEBUG_DRAWING
    debugDrawing(painter, cgo,cubic);
#endif

    // draw end points
    auto const &connectionStyle = QtNodes::StyleCollection::connectionStyle();

    double const pointDiameter = connectionStyle.pointDiameter();

    painter->setPen(connectionStyle.constructionColor());
    painter->setBrush(connectionStyle.constructionColor());
    double const pointRadius = pointDiameter / 2.0;
    if(connectionStyle.outArrow()) {
        auto out = createArrowPoly(cubic,pointRadius,pointDiameter * 1.5,false);
        painter->drawPolygon(out);
    } else {
        painter->drawEllipse(cgo.out(), pointRadius, pointRadius);
    }
    if(connectionStyle.inArrow()) {
        auto in = createArrowPoly(cubic,pointRadius,pointDiameter * 1.5,true);
        painter->drawPolygon(in);
    } else {
        painter->drawEllipse(cgo.in(), pointRadius, pointRadius);
    }
}

QPainterPath DefaultConnectionPainter::getPainterStroke(ConnectionGraphicsObject const &connection) const
{
    auto cubic = cubicPath(connection);

    QPointF const &out = connection.endPoint(PortType::Out);
    QPainterPath result(out);

    unsigned int constexpr segments = 20;

    for (auto i = 0ul; i < segments; ++i) {
        double ratio = double(i + 1) / segments;
        result.lineTo(cubic.pointAtPercent(ratio));
    }

    QPainterPathStroker stroker;
    stroker.setWidth(10.0);

    return stroker.createStroke(result);
}

#ifdef NODE_DEBUG_DRAWING
void DefaultConnectionPainter::debugDrawing(QPainter *painter, ConnectionGraphicsObject const &cgo,QPainterPath const & cubic)
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
        painter->drawPath(debugDrawing);
    }

    {
        painter->setPen(Qt::yellow);
        painter->drawRect(cgo.boundingRect());
    }
}
#endif

QPolygonF DefaultConnectionPainter::createArrowPoly(const QPainterPath& p, double mRadius,double arrowSize,bool drawIn) {
    float arrowStartPercentage;
    float arrowEndPercentage;

    if (drawIn) {
        arrowStartPercentage = p.percentAtLength(p.length() - mRadius - arrowSize);
        arrowEndPercentage = p.percentAtLength(p.length() - mRadius);
    }
    else {
        arrowStartPercentage = p.percentAtLength(mRadius + arrowSize);
        arrowEndPercentage = p.percentAtLength(mRadius);
    }
    QPointF headStartP = p.pointAtPercent(arrowStartPercentage);
    QPointF headEndP = p.pointAtPercent(arrowEndPercentage);
    QLineF arrowMiddleLine(headStartP, headEndP);
    QPointF normHead(arrowMiddleLine.dy(), -arrowMiddleLine.dx());
    QPointF arrowP1 = headStartP + normHead * 0.4;
    QPointF arrowP2 = headStartP - normHead * 0.4;

    QPolygonF arrowHeadEnd;
    arrowHeadEnd << headEndP << arrowP1 << arrowP2 /*<< headEndP*/;
    return arrowHeadEnd;
}

} // namespace QtNodes
