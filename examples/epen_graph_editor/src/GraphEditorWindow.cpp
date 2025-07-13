#include "GraphEditorWindow.hpp"
#include "FloatingToolbar.hpp"
#include <QAction>
#include <QApplication>
#include <QCursor>
#include <QDebug>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QtNodes/ConnectionStyle>
#include <QtNodes/StyleCollection>

using QtNodes::ConnectionStyle;
using QtNodes::NodeRole;
using QtNodes::StyleCollection;

GraphEditorWindow::GraphEditorWindow(BasicGraphicsScene *scene)
    : GraphicsView(scene)
    , m_toolbar(nullptr)
    , m_toolbarCreated(false)
    , _currentMode("pan")
{
    // Setup context menu
    setupNodeCreation();

    setWindowTitle("Simple Node Graph");
    resize(800, 600);

    setAcceptDrops(true);
    viewport()->setAcceptDrops(true); // Important for drag and drop

    // Don't create toolbar here - wait for showEvent
}

GraphEditorWindow::~GraphEditorWindow() {}

void GraphEditorWindow::showEvent(QShowEvent *event)
{
    GraphicsView::showEvent(event);

    // Create toolbar only when window is shown for the first time
    if (!m_toolbarCreated && isVisible()) {
        m_toolbarCreated = true;
        // Use a timer to ensure the window is fully rendered
        QTimer::singleShot(100, this, [this]() { createFloatingToolbar(); });
    }
}

void GraphEditorWindow::setupNodeCreation()
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    QAction *createNodeAction = new QAction(QStringLiteral("Create Node"), this);
    connect(createNodeAction, &QAction::triggered, this, &GraphEditorWindow::createNodeAtCursor);

    // Insert at the beginning of the actions list
    if (!actions().isEmpty()) {
        insertAction(actions().front(), createNodeAction);
    } else {
        addAction(createNodeAction);
    }
}

void GraphEditorWindow::createFloatingToolbar()
{
    qDebug() << "Creating floating toolbar...";
    qDebug() << "Main window geometry:" << geometry();
    qDebug() << "Main window visible:" << isVisible();

    // Create the floating toolbar
    m_toolbar = new FloatingToolbar(this);

    if (!m_toolbar) {
        qDebug() << "Failed to create toolbar!";
        return;
    }

    // Connect toolbar signals
    connect(m_toolbar, &FloatingToolbar::specificNodeRequested, this, &GraphEditorWindow::goToMode);

    // Connect other signals as needed
    connect(m_toolbar, &FloatingToolbar::fillColorChanged, [this](const QColor &color) {
        qDebug() << "Color changed to:" << color.name();
    });

    // Show the toolbar
    m_toolbar->show();
    m_toolbar->raise();

    // Set initial dock position (optional - start docked to right)
    // m_toolbar->setDockPosition(FloatingToolbar::DockedRight);

    qDebug() << "Toolbar created. Visible:" << m_toolbar->isVisible()
             << "Geometry:" << m_toolbar->geometry();
}

void GraphEditorWindow::createNodeAtPosition(const QPointF &scenePos)
{
    QtNodes::NodeId newId = nodeScene()->graphModel().addNode();
    nodeScene()->graphModel().setNodeData(newId, NodeRole::Position, scenePos);
}

void GraphEditorWindow::createNodeAtCursor()
{
    // Get mouse position in scene coordinates
    QPointF posView = mapToScene(mapFromGlobal(QCursor::pos()));
    createNodeAtPosition(posView);
}

void GraphEditorWindow::moveEvent(QMoveEvent *event)
{
    GraphicsView::moveEvent(event);

    // The toolbar will handle its own position updates through event filter
}

void GraphEditorWindow::resizeEvent(QResizeEvent *event)
{
    GraphicsView::resizeEvent(event);

    // Update toolbar position if it's docked
    if (m_toolbar && m_toolbar->isVisible() && m_toolbar->isDocked()) {
        m_toolbar->updatePosition();
    }
}

void GraphEditorWindow::goToMode(QString mode)
{
    _currentMode = mode;
    qDebug() << "Mode changed to:" << mode;
}

void GraphEditorWindow::mousePressEvent(QMouseEvent *event)
{
    GraphicsView::mousePressEvent(event);
}

void GraphEditorWindow::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << "DragEnter - MIME formats:" << event->mimeData()->formats();
    event->acceptProposedAction();
}

void GraphEditorWindow::dropEvent(QDropEvent *event)
{
    qDebug() << "Drop event - Text:" << event->mimeData()->text();
    event->acceptProposedAction();
    QPointF scenePos = mapToScene(event->position().toPoint());
    createNodeAtPosition(scenePos);
}

void GraphEditorWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}