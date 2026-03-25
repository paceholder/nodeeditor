#include "GraphicsView.hpp"

#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
#include "DataFlowGraphModel.hpp"
#include "Definitions.hpp"
#include "GroupGraphicsObject.hpp"
#include "NodeDelegateModel.hpp"
#include "NodeGraphicsObject.hpp"
#include "StyleCollection.hpp"
#include "UndoCommands.hpp"

#include <QtWidgets/QGraphicsScene>

#include <QtGui/QBrush>
#include <QtGui/QPen>

#include <QtWidgets/QMenu>

#include <QtCore/QDebug>
#include <QtCore/QPointF>
#include <QtCore/QRectF>

#include <QtOpenGL>
#include <QtWidgets>

#include <QtCore/QTimerEvent>

#include <algorithm>
#include <cmath>

namespace {
constexpr double zoom_friction = 0.75;
constexpr double zoom_impulse_per_step = 1.0;
constexpr double zoom_max_velocity = 5.0;
constexpr double zoom_per_notch = 1.05;
constexpr int zoom_timer_interval_ms = 16;
constexpr double zoom_velocity_epsilon = 0.001;

double zoom_base_k()
{
    static double const value = std::pow(zoom_per_notch,
                                         (1.0 - zoom_friction) / zoom_impulse_per_step);
    return value;
}

QPointF map_to_scene_exact(QGraphicsView const &view, QPointF const &viewPoint)
{
    bool invertible = false;
    QTransform const invertedTransform = view.viewportTransform().inverted(&invertible);
    if (!invertible) {
        return view.mapToScene(viewPoint.toPoint());
    }

    return invertedTransform.map(viewPoint);
}

QList<QtNodes::GraphicsView *> graphics_views(QGraphicsScene *scene)
{
    QList<QtNodes::GraphicsView *> result;
    if (!scene) {
        return result;
    }

    QList<QGraphicsView *> const views = scene->views();
    for (QGraphicsView *view : views) {
        if (auto *graphicsView = qobject_cast<QtNodes::GraphicsView *>(view)) {
            result.push_back(graphicsView);
        }
    }

    return result;
}

QGraphicsItem::CacheMode effective_node_cache_mode(QGraphicsScene *scene)
{
    QList<QtNodes::GraphicsView *> const views = graphics_views(scene);
    if (views.empty()) {
        return QGraphicsItem::DeviceCoordinateCache;
    }

    // Item cache mode is shared by all attached views. Mixed-view scenes keep node
    // rendering uncached so one view cannot force another into an incompatible mode.
    if (views.size() > 1) {
        return QGraphicsItem::NoCache;
    }

    QtNodes::GraphicsView const &view = *views.front();
    if (view.isZoomAnimating()) {
        return QGraphicsItem::NoCache;
    }

    return view.rasterizationPolicy() == QtNodes::GraphicsView::RasterizationPolicy::Consistent
        ? QGraphicsItem::NoCache
        : QGraphicsItem::DeviceCoordinateCache;
}

void set_node_cache_mode(
    QGraphicsScene *scene,
    QGraphicsItem::CacheMode mode,
    bool invalidate_cached_content = false)
{
    if (!scene) {
        return;
    }

    if (auto *nodeScene = dynamic_cast<QtNodes::BasicGraphicsScene *>(scene)) {
        for (auto const nodeId : nodeScene->graphModel().allNodeIds()) {
            if (auto *item = nodeScene->nodeGraphicsObject(nodeId)) {
                if (invalidate_cached_content && mode != QGraphicsItem::NoCache) {
                    item->setCacheMode(QGraphicsItem::NoCache);
                }
                item->setCacheMode(mode);
                item->update();
            }
        }
        return;
    }

    for (QGraphicsItem *item : scene->items()) {
        if (qgraphicsitem_cast<QtNodes::NodeGraphicsObject *>(item)) {
            if (invalidate_cached_content && mode != QGraphicsItem::NoCache) {
                item->setCacheMode(QGraphicsItem::NoCache);
            }
            item->setCacheMode(mode);
            item->update();
        }
    }
}

void refresh_node_cache_mode(QGraphicsScene *scene, bool invalidate_cached_content = false)
{
    set_node_cache_mode(scene,
                        effective_node_cache_mode(scene),
                        invalidate_cached_content);
}
} // namespace

using QtNodes::BasicGraphicsScene;
using QtNodes::DataFlowGraphModel;
using QtNodes::GraphicsView;
using QtNodes::NodeDelegateModel;
using QtNodes::NodeGraphicsObject;

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , _clearSelectionAction(Q_NULLPTR)
    , _deleteSelectionAction(Q_NULLPTR)
    , _cutSelectionAction(Q_NULLPTR)
    , _duplicateSelectionAction(Q_NULLPTR)
    , _copySelectionAction(Q_NULLPTR)
    , _pasteAction(Q_NULLPTR)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);

    auto const &flowViewStyle = StyleCollection::flowViewStyle();

    setBackgroundBrush(flowViewStyle.backgroundColor());

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    setCacheMode(QGraphicsView::CacheBackground);
    setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);

    setScaleRange(0.3, 2);

    // Sets the scene rect to its maximum possible ranges to avoid autu scene range
    // re-calculation when expanding the all QGraphicsItems common rect.
    int maxSize = 32767;
    setSceneRect(-maxSize, -maxSize, (maxSize * 2), (maxSize * 2));

    applyRasterizationPolicy();
}

GraphicsView::GraphicsView(BasicGraphicsScene *scene, QWidget *parent)
    : GraphicsView(parent)
{
    setScene(scene);
}

QAction *GraphicsView::clearSelectionAction() const
{
    return _clearSelectionAction;
}

QAction *GraphicsView::deleteSelectionAction() const
{
    return _deleteSelectionAction;
}

void GraphicsView::setScene(BasicGraphicsScene *scene)
{
    QGraphicsScene *oldScene = this->scene();

    QGraphicsView::setScene(scene);
    if (oldScene && oldScene != scene) {
        refresh_node_cache_mode(oldScene, true);
        oldScene->update();
    }

    if (!scene) {
        // Clear actions.
        delete _clearSelectionAction;
        delete _deleteSelectionAction;
        delete _duplicateSelectionAction;
        delete _copySelectionAction;
        delete _pasteAction;
        _clearSelectionAction = nullptr;
        _deleteSelectionAction = nullptr;
        _duplicateSelectionAction = nullptr;
        _copySelectionAction = nullptr;
        _pasteAction = nullptr;
        return;
    }

    applyRasterizationPolicy();

    {
        // setup actions
        delete _clearSelectionAction;
        _clearSelectionAction = new QAction(QStringLiteral("Clear Selection"), this);
        _clearSelectionAction->setShortcut(Qt::Key_Escape);

        connect(_clearSelectionAction, &QAction::triggered, scene, &QGraphicsScene::clearSelection);

        addAction(_clearSelectionAction);
    }

    {
        delete _deleteSelectionAction;
        _deleteSelectionAction = new QAction(QStringLiteral("Delete Selection"), this);
        _deleteSelectionAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
        _deleteSelectionAction->setShortcut(QKeySequence(QKeySequence::Delete));
        _deleteSelectionAction->setAutoRepeat(false);
        connect(_deleteSelectionAction,
                &QAction::triggered,
                this,
                &GraphicsView::onDeleteSelectedObjects);

        addAction(_deleteSelectionAction);
    }

    {
        delete _cutSelectionAction;
        _cutSelectionAction = new QAction(QStringLiteral("Cut Selection"), this);
        _cutSelectionAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
        _cutSelectionAction->setShortcut(QKeySequence(QKeySequence::Cut));
        _cutSelectionAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_X));
        _cutSelectionAction->setAutoRepeat(false);
        connect(_cutSelectionAction, &QAction::triggered, [this] {
            onCopySelectedObjects();
            onDeleteSelectedObjects();
        });

        addAction(_cutSelectionAction);
    }

    {
        delete _duplicateSelectionAction;
        _duplicateSelectionAction = new QAction(QStringLiteral("Duplicate Selection"), this);
        _duplicateSelectionAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
        _duplicateSelectionAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_D));
        _duplicateSelectionAction->setAutoRepeat(false);
        connect(_duplicateSelectionAction,
                &QAction::triggered,
                this,
                &GraphicsView::onDuplicateSelectedObjects);

        addAction(_duplicateSelectionAction);
    }

    {
        delete _copySelectionAction;
        _copySelectionAction = new QAction(QStringLiteral("Copy Selection"), this);
        _copySelectionAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
        _copySelectionAction->setShortcut(QKeySequence(QKeySequence::Copy));
        _copySelectionAction->setAutoRepeat(false);
        connect(_copySelectionAction,
                &QAction::triggered,
                this,
                &GraphicsView::onCopySelectedObjects);

        addAction(_copySelectionAction);
    }

    {
        delete _pasteAction;
        _pasteAction = new QAction(QStringLiteral("Paste Selection"), this);
        _pasteAction->setShortcutContext(Qt::ShortcutContext::WidgetShortcut);
        _pasteAction->setShortcut(QKeySequence(QKeySequence::Paste));
        _pasteAction->setAutoRepeat(false);
        connect(_pasteAction, &QAction::triggered, this, &GraphicsView::onPasteObjects);

        addAction(_pasteAction);
    }

    auto undoAction = scene->undoStack().createUndoAction(this, tr("&Undo"));
    undoAction->setShortcuts(QKeySequence::Undo);
    addAction(undoAction);

    auto redoAction = scene->undoStack().createRedoAction(this, tr("&Redo"));
    redoAction->setShortcuts(QKeySequence::Redo);
    addAction(redoAction);
}

void GraphicsView::centerScene()
{
    if (scene()) {
        scene()->setSceneRect(QRectF());

        QRectF sceneRect = scene()->sceneRect();

        if (sceneRect.width() > this->rect().width() || sceneRect.height() > this->rect().height()) {
            fitInView(sceneRect, Qt::KeepAspectRatio);
        }

        centerOn(sceneRect.center());
    }
}

void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
    QGraphicsView::contextMenuEvent(event);
    QMenu *menu = nullptr;
    const QPointF scenePos = mapToScene(event->pos());

    auto clickedItems = items(event->pos());

    for (QGraphicsItem *item : clickedItems) {
        if (auto *nodeItem = qgraphicsitem_cast<NodeGraphicsObject *>(item)) {
            Q_UNUSED(nodeItem);
            menu = nodeScene()->createStdMenu(scenePos);
            break;
        }

        if (auto *groupItem = qgraphicsitem_cast<GroupGraphicsObject *>(item)) {
            menu = nodeScene()->createGroupMenu(scenePos, groupItem);
            break;
        }
    }

    if (!menu) {
        if (!clickedItems.empty()) {
            menu = nodeScene()->createStdMenu(scenePos);
        } else {
            menu = nodeScene()->createSceneMenu(scenePos);
        }
    }

    if (menu) {
        menu->exec(event->globalPos());
    }

    return;
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    QPoint delta = event->angleDelta();

    if (delta.y() == 0) {
        event->ignore();
        return;
    }

    double const steps = delta.y() / 120.0;
    _zoomVelocity = std::clamp(_zoomVelocity + steps * zoom_impulse_per_step,
                               -zoom_max_velocity, zoom_max_velocity);
    _zoomPivot = event->position();

    if (_zoomTimerId == 0 && std::abs(_zoomVelocity) >= zoom_velocity_epsilon) {
        // Seed the timestamp one reference interval in the past so the immediate
        // applyZoomStep() call below sees dt ≈ 1.0 and applies a full first step.
        _lastZoomStepTime = std::chrono::steady_clock::now()
                          - std::chrono::milliseconds(zoom_timer_interval_ms);
        _zoomTimerId = startTimer(zoom_timer_interval_ms);
        refresh_node_cache_mode(scene(), true);
    }

    applyZoomStep();

    event->accept();
}

void GraphicsView::timerEvent(QTimerEvent *event)
{
    if (event->timerId() == _zoomTimerId) {
        applyZoomStep();
    } else {
        QGraphicsView::timerEvent(event);
    }
}

double GraphicsView::zoomAnimationScaleFactor(double velocity, double elapsedTimerSteps)
{
    if (elapsedTimerSteps <= 0.0 || std::abs(velocity) < zoom_velocity_epsilon) {
        return 1.0;
    }

    double const velocityDecay = std::pow(zoom_friction, elapsedTimerSteps);
    double const integratedVelocity = std::abs(1.0 - zoom_friction) > 1e-12
        ? velocity * (1.0 - velocityDecay) / (1.0 - zoom_friction)
        : velocity * elapsedTimerSteps;

    return std::pow(zoom_base_k(), integratedVelocity);
}

double GraphicsView::zoomAnimationVelocityAfter(double velocity, double elapsedTimerSteps)
{
    if (elapsedTimerSteps <= 0.0 || std::abs(velocity) < zoom_velocity_epsilon) {
        return velocity;
    }

    return velocity * std::pow(zoom_friction, elapsedTimerSteps);
}

void GraphicsView::applyZoomStep()
{
    if (std::abs(_zoomVelocity) < zoom_velocity_epsilon) {
        stopZoomTimer();
        return;
    }

    auto now = std::chrono::steady_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(now - _lastZoomStepTime).count();
    _lastZoomStepTime = now;

    if (elapsed_ms <= 0.0) {
        elapsed_ms = zoom_timer_interval_ms;
    }

    double const dt = elapsed_ms / zoom_timer_interval_ms;
    double const factor = zoomAnimationScaleFactor(_zoomVelocity, dt);
    double const current_scale = transform().m11();
    double const new_scale = current_scale * factor;

    if (_scaleRange.maximum > 0 && new_scale > _scaleRange.maximum) {
        applyZoomFactor(_scaleRange.maximum / current_scale);
        stopZoomTimer();
        return;
    }
    if (_scaleRange.minimum > 0 && new_scale < _scaleRange.minimum) {
        applyZoomFactor(_scaleRange.minimum / current_scale);
        stopZoomTimer();
        return;
    }

    applyZoomFactor(factor);
    _zoomVelocity = zoomAnimationVelocityAfter(_zoomVelocity, dt);
}

void GraphicsView::applyZoomFactor(double factor)
{
    QPointF const scenePivot = map_to_scene_exact(*this, _zoomPivot);
    double const newScale = transform().m11() * factor;

    auto const savedAnchor = transformationAnchor();
    setTransformationAnchor(QGraphicsView::NoAnchor);

    QTransform scaledTransform;
    scaledTransform.scale(newScale, newScale);
    setTransform(scaledTransform, false);

    QPointF const pivotAfterScale = viewportTransform().map(scenePivot);
    QPointF const shift = pivotAfterScale - _zoomPivot;
    horizontalScrollBar()->setValue(horizontalScrollBar()->value() + qRound(shift.x()));
    verticalScrollBar()->setValue(verticalScrollBar()->value() + qRound(shift.y()));

    QPointF const pivotAfterScroll = viewportTransform().map(scenePivot);
    QPointF const residual = _zoomPivot - pivotAfterScroll;

    QTransform preciseTransform;
    preciseTransform.translate(residual.x(), residual.y());
    preciseTransform.scale(newScale, newScale);
    setTransform(preciseTransform, false);

    setTransformationAnchor(savedAnchor);

    Q_EMIT scaleChanged(newScale);
}

void GraphicsView::stopZoomTimer()
{
    bool const hadFractionalOffset = std::abs(transform().dx()) > 1e-6 || std::abs(transform().dy()) > 1e-6;
    bool const was_zoom_animating = (_zoomTimerId != 0);

    if (_zoomTimerId != 0) {
        killTimer(_zoomTimerId);
        _zoomTimerId = 0;
    }

    if (hadFractionalOffset && _rasterizationPolicy == RasterizationPolicy::Crisp) {
        QPointF const viewOrigin(0.0, 0.0);
        QPointF const sceneAtOrigin = map_to_scene_exact(*this, viewOrigin);
        double const s = transform().m11();
        QTransform clean;
        clean.scale(s, s);
        setTransform(clean, false);

        QPointF const originAfterCleanup = viewportTransform().map(sceneAtOrigin);
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() + qRound(originAfterCleanup.x()));
        verticalScrollBar()->setValue(verticalScrollBar()->value() + qRound(originAfterCleanup.y()));
    }

    _zoomVelocity = 0.0;

    if (was_zoom_animating) {
        refresh_node_cache_mode(scene(), true);
        if (scene()) {
            scene()->update();
        }
        viewport()->update();
    }
}

double GraphicsView::getScale() const
{
    return transform().m11();
}

bool GraphicsView::isZoomAnimating() const
{
    return _zoomTimerId != 0;
}

GraphicsView::TextRenderingPolicy GraphicsView::textRenderingPolicy() const
{
    return _textRenderingPolicy;
}

void GraphicsView::setTextRenderingPolicy(TextRenderingPolicy policy)
{
    if (_textRenderingPolicy == policy) {
        return;
    }

    _textRenderingPolicy = policy;

    refresh_node_cache_mode(scene(), true);
    if (scene()) {
        scene()->update();
    }
    viewport()->update();
}

GraphicsView::RasterizationPolicy GraphicsView::rasterizationPolicy() const
{
    return _rasterizationPolicy;
}

void GraphicsView::setRasterizationPolicy(RasterizationPolicy policy)
{
    if (_rasterizationPolicy == policy) {
        return;
    }

    _rasterizationPolicy = policy;
    applyRasterizationPolicy();
}

void GraphicsView::stopZoomAnimation()
{
    stopZoomTimer();
}

void GraphicsView::setScaleRange(double minimum, double maximum)
{
    if (maximum < minimum)
        std::swap(minimum, maximum);
    minimum = std::max(0.0, minimum);
    maximum = std::max(0.0, maximum);

    _scaleRange = {minimum, maximum};

    setupScale(transform().m11());
}

void GraphicsView::setScaleRange(ScaleRange range)
{
    setScaleRange(range.minimum, range.maximum);
}

void GraphicsView::scaleUp()
{
    stopZoomTimer();

    double const step = 1.2;
    double const factor = std::pow(step, 1.0);

    if (_scaleRange.maximum > 0) {
        QTransform t = transform();
        t.scale(factor, factor);
        if (t.m11() >= _scaleRange.maximum) {
            setupScale(t.m11());
            return;
        }
    }

    scale(factor, factor);
    Q_EMIT scaleChanged(transform().m11());
}

void GraphicsView::scaleDown()
{
    stopZoomTimer();

    double const step = 1.2;
    double const factor = std::pow(step, -1.0);

    if (_scaleRange.minimum > 0) {
        QTransform t = transform();
        t.scale(factor, factor);
        if (t.m11() <= _scaleRange.minimum) {
            setupScale(t.m11());
            return;
        }
    }

    scale(factor, factor);
    Q_EMIT scaleChanged(transform().m11());
}

void GraphicsView::setupScale(double scale)
{
    stopZoomTimer();

    scale = std::max(_scaleRange.minimum, std::min(_scaleRange.maximum, scale));

    if (scale <= 0)
        return;

    if (scale == transform().m11())
        return;

    QTransform matrix;
    matrix.scale(scale, scale);
    setTransform(matrix, false);

    Q_EMIT scaleChanged(scale);
}

void GraphicsView::onDeleteSelectedObjects()
{
    if (!nodeScene())
        return;

    nodeScene()->undoStack().push(new DeleteCommand(nodeScene()));
}

void GraphicsView::onDuplicateSelectedObjects()
{
    if (!nodeScene())
        return;

    QPointF const pastePosition = scenePastePosition();

    nodeScene()->undoStack().push(new CopyCommand(nodeScene()));
    nodeScene()->undoStack().push(new PasteCommand(nodeScene(), pastePosition));
}

void GraphicsView::onCopySelectedObjects()
{
    if (!nodeScene())
        return;

    nodeScene()->undoStack().push(new CopyCommand(nodeScene()));
}

void GraphicsView::onPasteObjects()
{
    if (!nodeScene())
        return;

    QPointF const pastePosition = scenePastePosition();
    nodeScene()->undoStack().push(new PasteCommand(nodeScene(), pastePosition));
}

void GraphicsView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Shift:
        setDragMode(QGraphicsView::RubberBandDrag);
        break;

    default:
        break;
    }

    QGraphicsView::keyPressEvent(event);
}

void GraphicsView::keyReleaseEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Shift:
        setDragMode(QGraphicsView::ScrollHandDrag);
        break;

    default:
        break;
    }
    QGraphicsView::keyReleaseEvent(event);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::NoButton) {
        stopZoomTimer();
    }

    QGraphicsView::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        _clickPos = mapToScene(event->pos());
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);

    if (!scene())
        return;

    if (scene()->mouseGrabberItem() == nullptr && event->buttons() == Qt::LeftButton) {
        // Make sure shift is not being pressed
        if ((event->modifiers() & Qt::ShiftModifier) == 0) {
            QPointF difference = _clickPos - mapToScene(event->pos());
            setSceneRect(sceneRect().translated(difference.x(), difference.y()));
        }
    }
}

void GraphicsView::drawBackground(QPainter *painter, const QRectF &r)
{
    QGraphicsView::drawBackground(painter, r);

    painter->setRenderHint(QPainter::Antialiasing, true);

    qreal x_offset = 0.0;
    qreal y_offset = 0.0;
    bool const crisp_grid = (_rasterizationPolicy == RasterizationPolicy::Crisp);

    if (crisp_grid) {
        QTransform const view_transform = transform();
        qreal const scale_x = std::abs(view_transform.m11());
        qreal const scale_y = std::abs(view_transform.m22());
        if (scale_x > 0.0) {
            x_offset = 0.5 / scale_x;
        }
        if (scale_y > 0.0) {
            y_offset = 0.5 / scale_y;
        }
    }

    auto drawGrid = [&](double gridStep) {
        QRect windowRect = rect();
        QPointF tl = mapToScene(windowRect.topLeft());
        QPointF br = mapToScene(windowRect.bottomRight());

        double left = std::floor(tl.x() / gridStep - 0.5);
        double right = std::floor(br.x() / gridStep + 1.0);
        double bottom = std::floor(tl.y() / gridStep - 0.5);
        double top = std::floor(br.y() / gridStep + 1.0);

        // vertical lines
        for (int xi = int(left); xi <= int(right); ++xi) {
            qreal const x = xi * gridStep + x_offset;
            QLineF line(x, bottom * gridStep, x, top * gridStep);

            painter->drawLine(line);
        }

        // horizontal lines
        for (int yi = int(bottom); yi <= int(top); ++yi) {
            qreal const y = yi * gridStep + y_offset;
            QLineF line(left * gridStep, y, right * gridStep, y);
            painter->drawLine(line);
        }
    };

    auto const &flowViewStyle = StyleCollection::flowViewStyle();

    QPen pfine(flowViewStyle.fineGridColor(), 1.0);
    pfine.setCosmetic(crisp_grid);

    painter->setPen(pfine);
    drawGrid(15);

    QPen p(flowViewStyle.coarseGridColor(), 1.0);
    p.setCosmetic(crisp_grid);

    painter->setPen(p);
    drawGrid(150);
}

void GraphicsView::showEvent(QShowEvent *event)
{
    QGraphicsView::showEvent(event);

    centerScene();
}

BasicGraphicsScene *GraphicsView::nodeScene()
{
    return dynamic_cast<BasicGraphicsScene *>(scene());
}

QPointF GraphicsView::scenePastePosition()
{
    QPoint origin = mapFromGlobal(QCursor::pos());

    QRect const viewRect = rect();
    if (!viewRect.contains(origin))
        origin = viewRect.center();

    return mapToScene(origin);
}

void GraphicsView::zoomFitAll()
{
    stopZoomTimer();

    fitInView(scene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void GraphicsView::zoomFitSelected()
{
    stopZoomTimer();

    if (scene()->selectedItems().count() > 0) {
        QRectF unitedBoundingRect{};

        for (QGraphicsItem *item : scene()->selectedItems()) {
            unitedBoundingRect = unitedBoundingRect.united(
                item->mapRectToScene(item->boundingRect()));
        }

        fitInView(unitedBoundingRect, Qt::KeepAspectRatio);
    }
}

void GraphicsView::applyRasterizationPolicy()
{
    if (_rasterizationPolicy == RasterizationPolicy::Consistent) {
        setCacheMode(QGraphicsView::CacheNone);
    }
    else {
        if (std::abs(transform().dx()) > 1e-6 || std::abs(transform().dy()) > 1e-6) {
            QPointF const viewOrigin(0.0, 0.0);
            QPointF const sceneAtOrigin = map_to_scene_exact(*this, viewOrigin);
            double const s = transform().m11();
            QTransform clean;
            clean.scale(s, s);
            setTransform(clean, false);

            QPointF const originAfterCleanup = viewportTransform().map(sceneAtOrigin);
            horizontalScrollBar()->setValue(horizontalScrollBar()->value() + qRound(originAfterCleanup.x()));
            verticalScrollBar()->setValue(verticalScrollBar()->value() + qRound(originAfterCleanup.y()));
        }

        setCacheMode(QGraphicsView::CacheBackground);
    }

    refresh_node_cache_mode(scene(), true);
    if (scene()) {
        scene()->update();
    }
    viewport()->update();
}
