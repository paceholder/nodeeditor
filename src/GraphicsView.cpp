#include "GraphicsView.hpp"

#include "BasicGraphicsScene.hpp"
#include "ConnectionGraphicsObject.hpp"
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

#include <cmath>
#include <iostream>

using QtNodes::BasicGraphicsScene;
using QtNodes::GraphicsView;

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView(parent)
    , _clearSelectionAction(Q_NULLPTR)
    , _deleteSelectionAction(Q_NULLPTR)
    , _duplicateSelectionAction(Q_NULLPTR)
    , _copySelectionAction(Q_NULLPTR)
    , _pasteAction(Q_NULLPTR)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setRenderHint(QPainter::Antialiasing);

    auto const &flowViewStyle = StyleCollection::flowViewStyle();

    setBackgroundBrush(flowViewStyle.BackgroundColor);

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
    QGraphicsView::setScene(scene);

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
    if (itemAt(event->pos())) {
        QGraphicsView::contextMenuEvent(event);
        return;
    }

    auto const scenePos = mapToScene(event->pos());

    QMenu *menu = nodeScene()->createSceneMenu(scenePos);

    if (menu) {
        menu->exec(event->globalPos());
    }
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    QPoint delta = event->angleDelta();

    if (delta.y() == 0) {
        event->ignore();
        return;
    }

    double const d = delta.y() / std::abs(delta.y());

    if (d > 0.0)
        scaleUp();
    else
        scaleDown();
}

double GraphicsView::getScale() const
{
    return transform().m11();
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
    nodeScene()->undoStack().push(new DeleteCommand(nodeScene()));
}

void GraphicsView::onDuplicateSelectedObjects()
{
    QPointF const pastePosition = scenePastePosition();

    nodeScene()->undoStack().push(new CopyCommand(nodeScene()));
    nodeScene()->undoStack().push(new PasteCommand(nodeScene(), pastePosition));
}

void GraphicsView::onCopySelectedObjects()
{
    nodeScene()->undoStack().push(new CopyCommand(nodeScene()));
}

void GraphicsView::onPasteObjects()
{
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
    QGraphicsView::mousePressEvent(event);
    if (event->button() == Qt::LeftButton) {
        _clickPos = mapToScene(event->pos());
    }
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsView::mouseMoveEvent(event);
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
            QLineF line(xi * gridStep, bottom * gridStep, xi * gridStep, top * gridStep);

            painter->drawLine(line);
        }

        // horizontal lines
        for (int yi = int(bottom); yi <= int(top); ++yi) {
            QLineF line(left * gridStep, yi * gridStep, right * gridStep, yi * gridStep);
            painter->drawLine(line);
        }
    };

    auto const &flowViewStyle = StyleCollection::flowViewStyle();

    QPen pfine(flowViewStyle.FineGridColor, 1.0);

    painter->setPen(pfine);
    drawGrid(15);

    QPen p(flowViewStyle.CoarseGridColor, 1.0);

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
