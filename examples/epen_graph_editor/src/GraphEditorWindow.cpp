#include "GraphEditorWindow.hpp"
#include "FloatingToolbar.hpp"
#include "SimpleGraphModel.hpp"
#include <QtNodes/BasicGraphicsScene>
#include <QtNodes/ConnectionStyle>
#include <QtNodes/StyleCollection>
#include <QApplication>
#include <QAction>
#include <QCursor>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QTimer>
#include <QDebug>

using QtNodes::BasicGraphicsScene;
using QtNodes::ConnectionStyle;
using QtNodes::NodeRole;
using QtNodes::StyleCollection;

GraphEditorWindow::GraphEditorWindow()
    : GraphicsView()
    , m_toolbar(nullptr)
    , m_toolbarCreated(false)
{
    // Create the graph model
    m_graphModel = new SimpleGraphModel();

    // Create and set the scene
    auto scene = new BasicGraphicsScene(*m_graphModel);
    setScene(scene);
    
    // Setup context menu
    setupNodeCreation();
    
    setWindowTitle("Simple Node Graph");
    resize(800, 600);
    
    // Don't create toolbar here - wait for showEvent
}

GraphEditorWindow::~GraphEditorWindow()
{
    // The toolbar will be automatically deleted as it's a child of this window
    delete m_graphModel;
}

void GraphEditorWindow::showEvent(QShowEvent *event)
{
    GraphicsView::showEvent(event);
    
    // Create toolbar only when window is shown for the first time
    if (!m_toolbarCreated && isVisible()) {
        m_toolbarCreated = true;
        // Use a timer to ensure the window is fully rendered
        QTimer::singleShot(100, this, [this]() {
            createFloatingToolbar();
        });
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
    
    // Create the floating toolbar as a child widget
    m_toolbar = new FloatingToolbar(this);
    
    if (!m_toolbar) {
        qDebug() << "Failed to create toolbar!";
        return;
    }
    
    // Connect toolbar signals
    connect(m_toolbar, &FloatingToolbar::nodeRequested, 
            this, &GraphEditorWindow::createNodeAtCursor);
    
    // Connect other signals as needed
    connect(m_toolbar, &FloatingToolbar::fillColorChanged, [this](const QColor &color) {
        qDebug() << "Color changed to:" << color.name();
    });
    
    // Show the toolbar
    m_toolbar->show();
    m_toolbar->raise();
    
    // Update position
    m_toolbar->updatePosition();
    
    qDebug() << "Toolbar created. Visible:" << m_toolbar->isVisible() 
             << "Geometry:" << m_toolbar->geometry();
}

void GraphEditorWindow::createNodeAtPosition(const QPointF &scenePos)
{
    if (m_graphModel) {
        NodeId const newId = m_graphModel->addNode();
        m_graphModel->setNodeData(newId, NodeRole::Position, scenePos);
    }
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
    
    // No need to update toolbar position as it's now a child widget
}

void GraphEditorWindow::resizeEvent(QResizeEvent *event)
{
    GraphicsView::resizeEvent(event);
    
    // Update toolbar position when main window resizes
    if (m_toolbar && m_toolbar->isVisible()) {
        m_toolbar->updatePosition();
    }
}