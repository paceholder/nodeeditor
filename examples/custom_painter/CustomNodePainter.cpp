#include "CustomNodePainter.hpp"

#include <QtNodes/AbstractGraphModel>
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/StyleCollection>
#include <QtNodes/internal/NodeGraphicsObject.hpp>

#include <QPainterPath>

using QtNodes::AbstractGraphModel;
using QtNodes::AbstractNodeGeometry;
using QtNodes::BasicGraphicsScene;
using QtNodes::NodeRole;
using QtNodes::PortType;
using QtNodes::StyleCollection;

void CustomNodePainter::paint(QPainter *painter, NodeGraphicsObject &ngo) const
{
    painter->setRenderHint(QPainter::Antialiasing);

    drawBackground(painter, ngo);
    drawCaption(painter, ngo);
    drawPorts(painter, ngo);
}

void CustomNodePainter::drawBackground(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractNodeGeometry const &geometry = ngo.nodeScene()->nodeGeometry();
    AbstractGraphModel const &model = ngo.graphModel();
    QtNodes::NodeId const nodeId = ngo.nodeId();

    QRectF const boundingRect = geometry.boundingRect(nodeId);

    // Custom rounded rectangle with gradient
    double const radius = 15.0;

    QPainterPath path;
    path.addRoundedRect(boundingRect, radius, radius);

    // Create a custom gradient - blue to purple
    QLinearGradient gradient(boundingRect.topLeft(), boundingRect.bottomRight());

    if (ngo.isSelected()) {
        gradient.setColorAt(0.0, QColor(100, 149, 237)); // Cornflower blue
        gradient.setColorAt(1.0, QColor(186, 85, 211));  // Medium orchid
    } else {
        gradient.setColorAt(0.0, QColor(70, 130, 180)); // Steel blue
        gradient.setColorAt(1.0, QColor(138, 43, 226)); // Blue violet
    }

    painter->fillPath(path, gradient);

    // Draw border
    QPen pen(ngo.isSelected() ? QColor(255, 215, 0) : QColor(255, 255, 255), 2.0);
    painter->setPen(pen);
    painter->drawPath(path);
}

void CustomNodePainter::drawCaption(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractNodeGeometry const &geometry = ngo.nodeScene()->nodeGeometry();
    AbstractGraphModel const &model = ngo.graphModel();
    QtNodes::NodeId const nodeId = ngo.nodeId();

    QString const caption = model.nodeData(nodeId, NodeRole::Caption).toString();
    QRectF const boundingRect = geometry.boundingRect(nodeId);

    // Draw caption centered at top
    QFont font = painter->font();
    font.setBold(true);
    font.setPointSize(12);
    painter->setFont(font);

    painter->setPen(Qt::white);

    QRectF captionRect = boundingRect;
    captionRect.setHeight(30);

    painter->drawText(captionRect, Qt::AlignCenter, caption);
}

void CustomNodePainter::drawPorts(QPainter *painter, NodeGraphicsObject &ngo) const
{
    AbstractNodeGeometry const &geometry = ngo.nodeScene()->nodeGeometry();
    AbstractGraphModel const &model = ngo.graphModel();
    QtNodes::NodeId const nodeId = ngo.nodeId();

    // Draw input ports (left side) - green circles
    unsigned int const inPortCount = model.nodeData<unsigned int>(nodeId, NodeRole::InPortCount);
    for (unsigned int i = 0; i < inPortCount; ++i) {
        QPointF pos = geometry.portPosition(nodeId, PortType::In, i);
        painter->setBrush(QColor(50, 205, 50)); // Lime green
        painter->setPen(QPen(Qt::white, 1.5));
        painter->drawEllipse(pos, 6.0, 6.0);
    }

    // Draw output ports (right side) - orange circles
    unsigned int const outPortCount = model.nodeData<unsigned int>(nodeId, NodeRole::OutPortCount);
    for (unsigned int i = 0; i < outPortCount; ++i) {
        QPointF pos = geometry.portPosition(nodeId, PortType::Out, i);
        painter->setBrush(QColor(255, 165, 0)); // Orange
        painter->setPen(QPen(Qt::white, 1.5));
        painter->drawEllipse(pos, 6.0, 6.0);
    }
}
