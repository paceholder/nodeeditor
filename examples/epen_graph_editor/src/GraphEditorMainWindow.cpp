#include "GraphEditorMainWindow.hpp"
#include "FloatingCodeEditor.hpp"
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
    , m_codeEditor(nullptr)
    , m_toolbarCreated(false)
    , m_propertiesCreated(false)
    , m_codeEditorCreated(false)
    , _model(model)
    , m_currentSelectedNodeId(InvalidNodeId)
{
    setWindowTitle("Graph Editor");
    resize(1024, 768);

    setAcceptDrops(true);
    viewport()->setAcceptDrops(true); // Important for drag and drop

    // Connect to scene signals for node selection
    connect(scene, &BasicGraphicsScene::nodeSelected, this, [this](QtNodes::NodeId nodeId) {
        onNodeSelected(nodeId);
    });

    connect(_model, &DataFlowModel::nodePortSelected, this, &GraphEditorWindow::nodePortSelected);

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

    // Create code editor panel
    if (!m_codeEditorCreated && isVisible()) {
        m_codeEditorCreated = true;
        // Create code editor panel after other panels
        QTimer::singleShot(200, this, [this]() { createFloatingCodeEditor(); });
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

    // Update code editor position if it's docked
    if (m_codeEditor && m_codeEditor->isVisible() && m_codeEditor->isDocked()) {
        m_codeEditor->updatePosition();
    }
}

void GraphEditorWindow::mousePressEvent(QMouseEvent *event)
{
    GraphicsView::mousePressEvent(event);

    // Example: Check if a node was clicked and select it
    // You'll need to implement this based on your scene's node handling
    QGraphicsItem *item = scene()->itemAt(mapToScene(event->pos()), QTransform());
    if (item) {
    } else {
        onNodeDeselected();
    }
}

void GraphEditorWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->text() == "Process") {
        _allowedDropAreaLeft = _model->getlastProcessLeft();
        translate(0.01, 0.01);
    }
    event->acceptProposedAction();
}

void GraphEditorWindow::dragLeaveEvent(QDragLeaveEvent *event)
{
    _allowedDropAreaLeft = -1;
    translate(0.01, 0.01);
    event->accept();
}

void GraphEditorWindow::dropEvent(QDropEvent *event)
{
    event->acceptProposedAction();
    QPointF scenePos = mapToScene(event->position().toPoint());
    createNodeAtPosition(scenePos, event->mimeData()->text());
    _allowedDropAreaLeft = -1;
    translate(0.01, 0.01);
}

void GraphEditorWindow::dragMoveEvent(QDragMoveEvent *event)
{
    if (_allowedDropAreaLeft >= 0) {
        auto const scenePos = mapToScene(event->pos());
        if (scenePos.x() < _allowedDropAreaLeft) {
            event->ignore();
            return;
        }
    }
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

    // Show the properties panel
    m_properties->show();
    m_properties->raise();

    qDebug() << "Properties panel created. Visible:" << m_properties->isVisible()
             << "Geometry:" << m_properties->geometry();
}

void GraphEditorWindow::createFloatingCodeEditor()
{
    qDebug() << "Creating floating code editor...";

    // Create the floating code editor
    m_codeEditor = new FloatingCodeEditor(this);

    if (!m_codeEditor) {
        qDebug() << "Failed to create code editor!";
        return;
    }

    // Connect compile signal
    connect(m_codeEditor,
            &FloatingCodeEditor::compileRequested,
            this,
            &GraphEditorWindow::onCompileRequested);

    // Connect language change signal if needed
    connect(m_codeEditor, &FloatingCodeEditor::languageChanged, [this](const QString &language) {
        qDebug() << "Code editor language changed to:" << language;
        // Handle language change if needed
    });

    // Show the code editor
    m_codeEditor->show();
    m_codeEditor->raise();

    // Optionally start docked to bottom
    m_codeEditor->setDockPosition(FloatingPanelBase::DockedBottom);

    qDebug() << "Code editor created. Visible:" << m_codeEditor->isVisible()
             << "Geometry:" << m_codeEditor->geometry();
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
        m_properties->setObject(nodeModel);
    }
    if (_model) {
        _model->setSelectedNode(nodeModel, nodeId);
    }
}

void GraphEditorWindow::onNodeDeselected()
{
    m_currentSelectedNodeId = -1;

    if (m_properties) {
        m_properties->clearProperties();
    }
    if (_model) {
        _model->deselectNode();
    }
}

void GraphEditorWindow::onCompileRequested(const QString &code, const QString &language)
{
    qDebug() << "Compile requested for language:" << language;
    qDebug() << "Code length:" << code.length() << "characters";

    // Here you can add your actual compilation logic
    // For now, just log the request

    // Example: Send to your backend compiler or process the code
    if (language == "OpenCL") {
        // Handle OpenCL compilation
        qDebug() << "Compiling OpenCL kernel...";
    } else if (language == "CUDA") {
        // Handle CUDA compilation
        qDebug() << "Compiling CUDA kernel...";
    } else if (language == "Metal") {
        // Handle Metal compilation
        qDebug() << "Compiling Metal shader...";
    }

    // You can also update node properties or send to processing nodes
    if (_model) {
        // Example: _model->compileKernel(code, language);
    }
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

void GraphEditorWindow::nodePortSelected(bool isRightPort, Process *node, int portIndex)
{
    if (m_properties) {
        m_properties->setObject(node->findPort(portIndex, isRightPort));
    }
}