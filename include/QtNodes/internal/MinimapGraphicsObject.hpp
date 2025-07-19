#pragma once

#include <QtCore/QObject>
#include <QtGui/QPainter>
#include <QtWidgets/QGraphicsObject>

#include "Export.hpp"

namespace QtNodes {

class GraphicsView;
class BasicGraphicsScene;

class NODE_EDITOR_PUBLIC MinimapGraphicsObject : public QObject
{
    Q_OBJECT

public:
    MinimapGraphicsObject(GraphicsView *view);
    ~MinimapGraphicsObject();

    void setVisible(bool visible) { _visible = visible; }
    bool isVisible() const { return _visible; }

    void setOpacity(qreal opacity) { _opacity = opacity; }
    qreal opacity() const { return _opacity; }

    enum Corner {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight
    };

    void setCorner(Corner corner) { _corner = corner; }
    Corner corner() const { return _corner; }

    void setSize(QSizeF const &size) { _size = size; }
    QSizeF size() const { return _size; }

    void paint(QPainter *painter);

    bool handleMousePress(QPointF const &viewPos);
    bool handleMouseMove(QPointF const &viewPos);
    bool handleMouseRelease(QPointF const &viewPos);

Q_SIGNALS:
    void visibilityChanged(bool visible);

private:
    QRectF minimapRect() const;
    QRectF viewportRectInMinimap() const;
    QPointF sceneToMinimap(QPointF const &scenePoint) const;
    QPointF minimapToScene(QPointF const &minimapPoint) const;
    void navigateToPoint(QPointF const &minimapPoint);

private:
    GraphicsView *_view;
    bool _visible;
    qreal _opacity;
    Corner _corner;
    QSizeF _size;
    bool _dragging;
    QPointF _lastMousePos;

    static constexpr int MARGIN = 20;
    static constexpr qreal DEFAULT_OPACITY = 0.7;
};

} // namespace QtNodes