#include "GraphEditorMainWindow.hpp"
#include "FloatingProperties.hpp"
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

using QtNodes::ConnectionId;
using QtNodes::ConnectionStyle;
using QtNodes::NodeRole;
using QtNodes::StyleCollection;

GraphEditorWindow::GraphEditorWindow(DataFlowGraphicsScene *scene, DataFlowModel *model)
    : GraphicsView(scene)
    , m_toolbar(nullptr)
    , m_properties(nullptr)
    , m_toolbarCreated(false)
    , m_propertiesCreated(false)
    , _currentMode("pan")
    , _model(model)
    , m_currentSelectedNodeId(InvalidNodeId)
{
    // Setup context menu
    //setupNodeCreation();

    setWindowTitle("Simple Node Graph");
    resize(800, 600);

    setAcceptDrops(true);
    viewport()->setAcceptDrops(true); // Important for drag and drop

    // Connect to scene signals for node selection
    connect(scene, &BasicGraphicsScene::nodeSelected, this, [this](QtNodes::NodeId nodeId) {
        onNodeSelected(nodeId);
    });

    // Create initial nodes
    QtNodes::NodeId newIdInput = _model->addNode("VideoInput");
    QPointF inputScenePos = mapToScene(0, 0);
    _model->setNodeData(newIdInput, NodeRole::Position, inputScenePos);

    QtNodes::NodeId newIdOutput = _model->addNode("VideoOutput");
    QPointF outputScenePos = mapToScene(400, 0);
    _model->setNodeData(newIdOutput, NodeRole::Position, outputScenePos);

    _model->addConnection(ConnectionId{newIdInput, 0, newIdOutput, 0});

    setupScale(0.8);
}

GraphEditorWindow::~GraphEditorWindow()
{
    // Cleanup is handled by QPointer
}

void GraphEditorWindow::showEvent(QShowEvent *event)
{
    GraphicsView::showEvent(event);

    // Create toolbar only when window is shown for the first time
    if (!m_toolbarCreated && isVisible()) {
        m_toolbarCreated = true;
        // Use a timer to ensure the window is fully rendered
        QTimer::singleShot(100, this, [this]() { createFloatingToolbar(); });
    }

    // Create properties panel
    if (!m_propertiesCreated && isVisible()) {
        m_propertiesCreated = true;
        // Create properties panel slightly after toolbar
        QTimer::singleShot(150, this, [this]() { createFloatingProperties(); });
    }
}

void GraphEditorWindow::moveEvent(QMoveEvent *event)
{
    GraphicsView::moveEvent(event);
    // The toolbars will handle their own position updates through event filter
}

void GraphEditorWindow::resizeEvent(QResizeEvent *event)
{
    GraphicsView::resizeEvent(event);

    // Update toolbar position if it's docked
    if (m_toolbar && m_toolbar->isVisible() && m_toolbar->isDocked()) {
        m_toolbar->updatePosition();
    }

    // Update properties position if it's docked
    if (m_properties && m_properties->isVisible() && m_properties->isDocked()) {
        m_properties->updatePosition();
    }
}

void GraphEditorWindow::mousePressEvent(QMouseEvent *event)
{
    GraphicsView::mousePressEvent(event);

    // Example: Check if a node was clicked and select it
    // You'll need to implement this based on your scene's node handling
    QGraphicsItem *item = scene()->itemAt(mapToScene(event->pos()), QTransform());
    if (item) {
        // Check if this is a node item and get its ID
        // This is a simplified example - adjust based on your actual node implementation
        // auto nodeItem = dynamic_cast<NodeGraphicsObject*>(item);
        // if (nodeItem) {
        //     onNodeSelected(nodeItem->nodeId());
        // }
    } else {
        onNodeDeselected();
    }
}

void GraphEditorWindow::dragEnterEvent(QDragEnterEvent *event)
{
    event->acceptProposedAction();
}

void GraphEditorWindow::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();
    QPointF scenePos = mapToScene(event->position().toPoint());
    createNodeAtPosition(scenePos, event->mimeData()->text());
}

void GraphEditorWindow::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
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

    // Connect other signals as needed
    connect(m_toolbar, &FloatingToolbar::fillColorChanged, [this](const QColor &color) {
        qDebug() << "Color changed to:" << color.name();
    });

    // Show the toolbar
    m_toolbar->show();
    m_toolbar->raise();

    // Set initial dock position (optional - start docked to left)
    // m_toolbar->setDockPosition(FloatingToolbar::DockedLeft);

    qDebug() << "Toolbar created. Visible:" << m_toolbar->isVisible()
             << "Geometry:" << m_toolbar->geometry();
}

void GraphEditorWindow::createFloatingProperties()
{
    qDebug() << "Creating floating properties panel...";

    // Create the floating properties panel
    m_properties = new FloatingProperties(this);

    _model->setFloatingProperties(m_properties);
    if (!m_properties) {
        qDebug() << "Failed to create properties panel!";
        return;
    }

    // Connect to the properties panel's node selection signals if needed
    connect(m_properties,
            &FloatingProperties::nodeSelected,
            this,
            &GraphEditorWindow::onNodeSelected);
    connect(m_properties,
            &FloatingProperties::nodeDeselected,
            this,
            &GraphEditorWindow::onNodeDeselected);

    // If you have node selection in your scene, connect it
    // Example (adjust based on your actual implementation):
    // auto dataFlowScene = dynamic_cast<DataFlowGraphicsScene*>(scene());
    // if (dataFlowScene) {
    //     connect(dataFlowScene, &DataFlowGraphicsScene::nodeSelected,
    //             this, &GraphEditorWindow::onNodeSelected);
    //     connect(dataFlowScene, &DataFlowGraphicsScene::nodeDeselected,
    //             this, &GraphEditorWindow::onNodeDeselected);
    // }

    // Show the properties panel
    m_properties->show();
    m_properties->raise();

    // Start docked to the right by default
    // m_properties->setDockPosition(FloatingProperties::DockedRight);

    qDebug() << "Properties panel created. Visible:" << m_properties->isVisible()
             << "Geometry:" << m_properties->geometry();
}

void GraphEditorWindow::createNodeAtPosition(const QPointF &scenePos, const QString nodeType)
{
    QtNodes::NodeId newId = _model->addNode(nodeType);
    _model->setNodeData(newId, NodeRole::Position, scenePos);

    // The node selection will be handled by the scene's nodeSelected signal
}

void GraphEditorWindow::createNodeAtCursor()
{
    // Get mouse position in scene coordinates
    QPointF posView = mapToScene(mapFromGlobal(QCursor::pos()));
    createNodeAtPosition(posView, "ImageShowModel");
}

void GraphEditorWindow::onNodeSelected(NodeId nodeId)
{
    m_currentSelectedNodeId = nodeId;

    OperationDataModel *nodeModel = _model->delegateModel<OperationDataModel>(nodeId);
    if (m_properties) {
        m_properties->setNode(nodeModel);
    }
}

void GraphEditorWindow::onNodeDeselected()
{
    m_currentSelectedNodeId = -1;

    if (m_properties) {
        m_properties->clearProperties();
    }

    qDebug() << "Node deselected";
}

void GraphEditorWindow::drawBackground(QPainter *painter, const QRectF &r)
{
    GraphicsView::drawBackground(painter, r);

    if (_allowedDropAreaLeft != -1) {
        float left = _allowedDropAreaLeft;
        if (left == -2)
            left = r.left();
        if (left <= r.right()) {
            if (left < r.left())
                left = r.left();
            // Set anti-aliasing (optional for smoother edges)
            painter->setRenderHint(QPainter::Antialiasing);

            QColor semiTransparentBlue(0, 0, 255, 50);

            // Set brush with the color
            painter->setBrush(QBrush(semiTransparentBlue));

            // Set pen to no border (optional)
            painter->setPen(Qt::NoPen);

            // Draw the rectangle
            painter->drawRect(QRectF(left, r.top(), r.right() - left, r.height()));
        }
    }
}