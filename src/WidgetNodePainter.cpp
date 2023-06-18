#include "WidgetNodePainter.hpp"

#include <cmath>


#include "QtNodes/NodeColors.hpp"
#include "QtNodes/DataColors.hpp"
#include "QtNodes/AbstractGraphModel"
#include "QtNodes/internal/AbstractNodeGeometry.hpp"
#include "QtNodes/internal/NodeStyle.hpp"
#include "QtNodes/NodeData"
#include "QtNodes/internal/NodeState.hpp"
#include "AbstractGraphModel.hpp"
#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "ConnectionIdUtils.hpp"
#include "NodeGraphicsObject.hpp"
#include <QApplication>

QtNodes::WidgetNodePainter::WidgetNodePainter() : AbstractNodePainter() {
}

void QtNodes::WidgetNodePainter::paint(QPainter *painter, NodeGraphicsObject &ngo) const {

    drawNodeBackground(painter, ngo);
    drawNodeCaption(painter, ngo);
    drawNodeBoundary(painter, ngo);

    drawConnectionPoints(painter, ngo);

    drawFilledConnectionPoints(painter, ngo);

    drawResizeRect(painter, ngo);
}

void QtNodes::WidgetNodePainter::drawNodeBackground(QPainter *painter, NodeGraphicsObject &ngo) const {
    AbstractGraphModel &model = ngo.graphModel();

    NodeId const nodeId = ngo.nodeId();

    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();
    QSize size = geometry.size(nodeId);

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));

    NodeStyle nodeStyle(json.object());

    QPen p(nodeStyle.NormalBoundaryColor, 0);
    painter->setPen(p);

    QLinearGradient gradient(QPointF(0.0, 0.0), QPointF(0, size.height()));

    QPalette palette = QApplication::palette();
    gradient.setColorAt(0.0, palette.color(QPalette::Base));
    gradient.setColorAt(0.10, palette.color(QPalette::AlternateBase));
    gradient.setColorAt(0.90, palette.color(QPalette::AlternateBase));
    gradient.setColorAt(1.0, palette.color(QPalette::Window));

    painter->setBrush(gradient);

    QRectF boundary(0, 0, size.width(), size.height());

    double const radius = 3.0;
    painter->drawRoundedRect(boundary, radius, radius);
}

void QtNodes::WidgetNodePainter::drawNodeBoundary(QPainter *painter, NodeGraphicsObject &ngo) const {
    QColor col = ngo.isSelected() ? QColor(255, 255, 255) : QColor(0, 0, 0);
    int width = ngo.isSelected() ? 2 : 0;

    NodeId const nodeId = ngo.nodeId();

    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QSize size = geometry.size(nodeId);

    QRectF boundary(0, 0, size.width(), size.height());

    double const radius = 3.0;
    QBrush brush(Qt::transparent);
    painter->setBrush(brush);
    QPen borderPen(col);
    borderPen.setWidth(width);
    painter->setPen(borderPen);
    painter->drawRoundedRect(boundary, radius, radius);
}

void QtNodes::WidgetNodePainter::drawConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const {
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
    NodeStyle nodeStyle(json.object());

    float diameter = nodeStyle.ConnectionPointDiameter;
    auto reducedDiameter = diameter * 0.6;

    for (PortType portType: {PortType::Out, PortType::In}) {
        size_t const n = model
                .nodeData(nodeId,
                          (portType == PortType::Out) ? NodeRole::OutPortCount
                                                      : NodeRole::InPortCount)
                .toUInt();

        for (PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            QPointF p = geometry.portPosition(nodeId, portType, portIndex);

            auto const &dataType = model.portData(nodeId, portType, portIndex, PortRole::DataType)
                    .value<NodeDataType>();

            double r = 1.0;

            NodeState const &state = ngo.nodeState();

            if (auto const *cgo = state.connectionForReaction()) {
                PortType requiredPort = cgo->connectionState().requiredPort();

                if (requiredPort == portType) {
                    ConnectionId possibleConnectionId = makeCompleteConnectionId(cgo->connectionId(),
                                                                                 nodeId,
                                                                                 portIndex);

                    bool const possible = model.connectionPossible(possibleConnectionId);

                    auto cp = cgo->sceneTransform().map(cgo->endPoint(requiredPort));
                    cp = ngo.sceneTransform().inverted().map(cp);

                    auto diff = cp - p;
                    double dist = std::sqrt(QPointF::dotProduct(diff, diff));

                    if (possible) {
                        double const thres = 40.0;
                        r = (dist < thres) ? (2.0 - dist / thres) : 1.0;
                    } else {
                        double const thres = 80.0;
                        r = (dist < thres) ? (dist / thres) : 1.0;
                    }
                }
            }
            painter->setPen({0, 0, 0});
            painter->setBrush(DataColors::getColor(dataType.id));

            painter->drawEllipse(p, reducedDiameter * r, reducedDiameter * r);
        }
    }

    if (ngo.nodeState().connectionForReaction()) {
        ngo.nodeState().resetConnectionForReaction();
    }
}

void QtNodes::WidgetNodePainter::drawFilledConnectionPoints(QPainter *painter, NodeGraphicsObject &ngo) const {
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
    NodeStyle nodeStyle(json.object());

    auto diameter = nodeStyle.ConnectionPointDiameter;

    for (PortType portType: {PortType::Out, PortType::In}) {
        size_t const n = model
                .nodeData(nodeId,
                          (portType == PortType::Out) ? NodeRole::OutPortCount
                                                      : NodeRole::InPortCount)
                .toUInt();

        for (PortIndex portIndex = 0; portIndex < n; ++portIndex) {
            QPointF p = geometry.portPosition(nodeId, portType, portIndex);

            auto const &connected = model.connections(nodeId, portType, portIndex);

            if (!connected.empty()) {
                auto const &dataType = model
                        .portData(nodeId, portType, portIndex, PortRole::DataType)
                        .value<NodeDataType>();


                painter->setPen(DataColors::getColor(dataType.id));
                painter->setBrush(DataColors::getColor(dataType.id));
                painter->drawEllipse(p, diameter * 0.4, diameter * 0.4);
            }
        }
    }
}

void QtNodes::WidgetNodePainter::drawNodeCaption(QPainter *painter, NodeGraphicsObject &ngo) const {
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    if (!model.nodeData(nodeId, NodeRole::CaptionVisible).toBool())
        return;

    QString const name = model.nodeData(nodeId, NodeRole::Caption).toString();

    QFont f = painter->font();
    f.setBold(true);

    QPointF position = geometry.captionPosition(nodeId);
    position.setX(10);
    position.setY(16);

    QJsonDocument json = QJsonDocument::fromVariant(model.nodeData(nodeId, NodeRole::Style));
    NodeStyle nodeStyle(json.object());


    QSize size = geometry.size(nodeId);
    QRectF boundary(0, 0, size.width(), 22);
    QColor captionCol = NodeColors::getColor(model.nodeData(nodeId, NodeRole::Type).value<QString>());
    painter->setPen(captionCol);
    double const radius = 3.0;
    painter->setBrush(QBrush(captionCol));
    painter->drawRoundedRect(boundary, radius, radius);

    painter->setFont(f);
    painter->setPen(nodeStyle.FontColor);
    painter->drawText(position, name);


    f.setBold(false);
    painter->setFont(f);
}

void QtNodes::WidgetNodePainter::drawResizeRect(QPainter *painter, NodeGraphicsObject &ngo) const {
    AbstractGraphModel &model = ngo.graphModel();
    NodeId const nodeId = ngo.nodeId();
    AbstractNodeGeometry &geometry = ngo.nodeScene()->nodeGeometry();

    if (model.nodeFlags(nodeId) & NodeFlag::Resizable) {
        painter->setBrush(Qt::gray);

        painter->drawEllipse(geometry.resizeHandleRect(nodeId));
    }
}
