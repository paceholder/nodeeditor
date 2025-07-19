#include "MinimapGraphicsObject.hpp"

#include "BasicGraphicsScene.hpp"
#include "GraphicsView.hpp"
#include "NodeGraphicsObject.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "CommentGraphicsObject.hpp"
#include "StyleCollection.hpp"

#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QApplication>

#include <algorithm>

namespace QtNodes {

MinimapGraphicsObject::MinimapGraphicsObject(GraphicsView *view)
    : QObject(view)
    , _view(view)
    , _visible(true)
    , _opacity(DEFAULT_OPACITY)
    , _corner(TopRight)
    , _size(200, 150)
    , _dragging(false)
{
}

MinimapGraphicsObject::~MinimapGraphicsObject() = default;

QRectF MinimapGraphicsObject::minimapRect() const
{
    if (!_view || !_view->scene()) return QRectF();

    QRect viewportRect = _view->viewport()->rect();
    QPointF topLeft;
    
    switch (_corner) {
    case TopLeft:
        topLeft = QPointF(MARGIN, MARGIN);
        break;
    case TopRight:
        topLeft = QPointF(viewportRect.width() - _size.width() - MARGIN, MARGIN);
        break;
    case BottomLeft:
        topLeft = QPointF(MARGIN, viewportRect.height() - _size.height() - MARGIN);
        break;
    case BottomRight:
        topLeft = QPointF(viewportRect.width() - _size.width() - MARGIN,
                         viewportRect.height() - _size.height() - MARGIN);
        break;
    }
    
    return QRectF(topLeft, _size);
}

QRectF MinimapGraphicsObject::viewportRectInMinimap() const
{
    if (!_view || !_view->scene()) return QRectF();

    QRectF sceneRect = _view->scene()->itemsBoundingRect();
    if (sceneRect.isEmpty()) {
        sceneRect = QRectF(-1000, -1000, 2000, 2000); // Default bounds
    }

    // Add padding
    sceneRect.adjust(-100, -100, 100, 100);

    QRectF viewRect = _view->mapToScene(_view->viewport()->rect()).boundingRect();
    QRectF minimap = minimapRect();

    // Calculate scale to fit scene in minimap
    qreal scaleX = minimap.width() / sceneRect.width();
    qreal scaleY = minimap.height() / sceneRect.height();
    qreal scale = std::min(scaleX, scaleY);

    // Transform viewport rect to minimap coordinates
    QPointF viewTopLeft = (viewRect.topLeft() - sceneRect.topLeft()) * scale + minimap.topLeft();
    QSizeF viewSize = viewRect.size() * scale;

    return QRectF(viewTopLeft, viewSize);
}

QPointF MinimapGraphicsObject::sceneToMinimap(QPointF const &scenePoint) const
{
    if (!_view || !_view->scene()) return QPointF();

    QRectF sceneRect = _view->scene()->itemsBoundingRect();
    if (sceneRect.isEmpty()) {
        sceneRect = QRectF(-1000, -1000, 2000, 2000);
    }
    sceneRect.adjust(-100, -100, 100, 100);

    QRectF minimap = minimapRect();

    qreal scaleX = minimap.width() / sceneRect.width();
    qreal scaleY = minimap.height() / sceneRect.height();
    qreal scale = std::min(scaleX, scaleY);

    return (scenePoint - sceneRect.topLeft()) * scale + minimap.topLeft();
}

QPointF MinimapGraphicsObject::minimapToScene(QPointF const &minimapPoint) const
{
    if (!_view || !_view->scene()) return QPointF();

    QRectF sceneRect = _view->scene()->itemsBoundingRect();
    if (sceneRect.isEmpty()) {
        sceneRect = QRectF(-1000, -1000, 2000, 2000);
    }
    sceneRect.adjust(-100, -100, 100, 100);

    QRectF minimap = minimapRect();

    qreal scaleX = minimap.width() / sceneRect.width();
    qreal scaleY = minimap.height() / sceneRect.height();
    qreal scale = std::min(scaleX, scaleY);

    return (minimapPoint - minimap.topLeft()) / scale + sceneRect.topLeft();
}

void MinimapGraphicsObject::navigateToPoint(QPointF const &minimapPoint)
{
    QPointF scenePoint = minimapToScene(minimapPoint);
    _view->centerOn(scenePoint);
}

void MinimapGraphicsObject::paint(QPainter *painter)
{
    if (!_visible || !_view || !_view->scene()) return;

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);

    QRectF minimap = minimapRect();

    // Draw minimap background
    painter->setOpacity(_opacity);
    painter->fillRect(minimap, QColor(60, 60, 60, 180));
    painter->setPen(QPen(QColor(80, 80, 80), 1));
    painter->drawRect(minimap);

    // Get scene bounds
    QRectF sceneRect = _view->scene()->itemsBoundingRect();
    if (sceneRect.isEmpty()) {
        sceneRect = QRectF(-1000, -1000, 2000, 2000);
    }
    sceneRect.adjust(-100, -100, 100, 100);

    // Note: scale calculation moved to where it's needed

    // Set clipping to minimap area
    painter->setClipRect(minimap);

    // Draw simplified scene items
    painter->setOpacity(_opacity * 0.6);

    // Draw comments (lowest layer)
    auto scene = qobject_cast<BasicGraphicsScene*>(_view->scene());
    if (scene) {
        for (auto const& [commentId, comment] : scene->comments()) {
            if (comment) {
                QRectF commentRect = comment->boundingRect();
                QPointF commentPos = comment->pos();
                QRectF worldRect = commentRect.translated(commentPos);
                
                QPointF topLeft = sceneToMinimap(worldRect.topLeft());
                QPointF bottomRight = sceneToMinimap(worldRect.bottomRight());
                QRectF minimapCommentRect(topLeft, bottomRight);
                
                painter->fillRect(minimapCommentRect, comment->color());
                painter->setPen(QPen(comment->color().darker(150), 0.5));
                painter->drawRect(minimapCommentRect);
            }
        }
    }

    // Draw nodes
    for (QGraphicsItem *item : _view->scene()->items()) {
        if (auto node = qgraphicsitem_cast<NodeGraphicsObject*>(item)) {
            QRectF nodeRect = node->boundingRect();
            QPointF nodePos = node->pos();
            QRectF worldRect = nodeRect.translated(nodePos);
            
            QPointF topLeft = sceneToMinimap(worldRect.topLeft());
            QPointF bottomRight = sceneToMinimap(worldRect.bottomRight());
            QRectF minimapNodeRect(topLeft, bottomRight);
            
            // Ensure minimum size for visibility
            if (minimapNodeRect.width() < 2) minimapNodeRect.setWidth(2);
            if (minimapNodeRect.height() < 2) minimapNodeRect.setHeight(2);
            
            painter->fillRect(minimapNodeRect, QColor(120, 120, 120));
        }
    }

    // Draw connections as thin lines
    painter->setPen(QPen(QColor(100, 100, 100), 0.5));
    for (QGraphicsItem *item : _view->scene()->items()) {
        if (auto conn = qgraphicsitem_cast<ConnectionGraphicsObject*>(item)) {
            QPainterPath path = conn->shape();
            if (!path.isEmpty()) {
                QPointF start = sceneToMinimap(path.elementAt(0));
                QPointF end = sceneToMinimap(path.elementAt(path.elementCount() - 1));
                painter->drawLine(start, end);
            }
        }
    }

    // Draw viewport rectangle
    painter->setOpacity(1.0);
    QRectF viewportRect = viewportRectInMinimap();
    painter->setPen(QPen(QColor(200, 200, 200), 2));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(viewportRect);
    
    // Fill viewport with semi-transparent highlight
    painter->fillRect(viewportRect, QColor(200, 200, 200, 30));

    painter->restore();
}

bool MinimapGraphicsObject::handleMousePress(QPointF const &viewPos)
{
    if (!_visible) return false;

    QRectF minimap = minimapRect();
    
    if (minimap.contains(viewPos)) {
        _dragging = true;
        _lastMousePos = viewPos;
        navigateToPoint(viewPos);
        return true;
    }
    
    return false;
}

bool MinimapGraphicsObject::handleMouseMove(QPointF const &viewPos)
{
    if (!_visible || !_dragging) return false;

    navigateToPoint(viewPos);
    _lastMousePos = viewPos;
    
    return true;
}

bool MinimapGraphicsObject::handleMouseRelease(QPointF const &viewPos)
{
    Q_UNUSED(viewPos)
    
    if (_dragging) {
        _dragging = false;
        return true;
    }
    
    return false;
}

} // namespace QtNodes