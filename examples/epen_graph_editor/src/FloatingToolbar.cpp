#include "FloatingToolbar.hpp"
#include "GraphEditorWindow.hpp"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QShowEvent>
#include <QScreen>
#include <QApplication>
#include <QDebug>

FloatingToolbar::FloatingToolbar(GraphEditorWindow *parent)
    : QWidget(nullptr)  // Note: parent is nullptr for top-level window
    , m_graphEditor(parent)
{
    // Set window flags for a tool window
    setWindowFlags(Qt::Tool | Qt::WindowStaysOnTopHint);
    
    setWindowTitle("Tools");
    setFixedWidth(150);
    
    setupUI();
    connectSignals();
    
    // Set a minimum height to ensure visibility
    setMinimumHeight(400);
    
    // Position the toolbar initially
    if (parent) {
        positionRelativeToParent();
    }
}

void FloatingToolbar::setupUI()
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(5, 5, 5, 5);
    
    // Add title
    QLabel *title = new QLabel("Node Editor Tools");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-weight: bold; padding: 5px; background-color: #f0f0f0;");
    m_layout->addWidget(title);
    
    // Node creation section
    m_layout->addSpacing(10);
    QLabel *nodeLabel = new QLabel("Nodes:");
    nodeLabel->setStyleSheet("font-weight: bold;");
    m_layout->addWidget(nodeLabel);
    
    QPushButton *btnNode = new QPushButton("Create Node");
    btnNode->setToolTip("Create a new node at cursor position");
    m_layout->addWidget(btnNode);
    
    // Shape tools section (for future use)
    m_layout->addSpacing(10);
    QLabel *shapeLabel = new QLabel("Shapes:");
    shapeLabel->setStyleSheet("font-weight: bold;");
    m_layout->addWidget(shapeLabel);
    
    QPushButton *btnRect = new QPushButton("Rectangle");
    btnRect->setEnabled(false); // Disable for now
    m_layout->addWidget(btnRect);
    
    QPushButton *btnCircle = new QPushButton("Circle");
    btnCircle->setEnabled(false); // Disable for now
    m_layout->addWidget(btnCircle);
    
    // Color section
    m_layout->addSpacing(10);
    QLabel *colorLabel = new QLabel("Colors:");
    colorLabel->setStyleSheet("font-weight: bold;");
    m_layout->addWidget(colorLabel);
    
    QPushButton *btnRed = new QPushButton("Red Fill");
    btnRed->setStyleSheet("QPushButton { background-color: #ffcccc; }");
    m_layout->addWidget(btnRed);
    
    QPushButton *btnBlue = new QPushButton("Blue Fill");
    btnBlue->setStyleSheet("QPushButton { background-color: #ccccff; }");
    m_layout->addWidget(btnBlue);
    
    QPushButton *btnGreen = new QPushButton("Green Fill");
    btnGreen->setStyleSheet("QPushButton { background-color: #ccffcc; }");
    m_layout->addWidget(btnGreen);
    
    // View controls section
    m_layout->addSpacing(10);
    QLabel *viewLabel = new QLabel("View:");
    viewLabel->setStyleSheet("font-weight: bold;");
    m_layout->addWidget(viewLabel);
    
    QPushButton *btnZoomIn = new QPushButton("Zoom In");
    m_layout->addWidget(btnZoomIn);
    
    QPushButton *btnZoomOut = new QPushButton("Zoom Out");
    m_layout->addWidget(btnZoomOut);
    
    QPushButton *btnResetView = new QPushButton("Reset View");
    m_layout->addWidget(btnResetView);
    
    // Add stretch to push everything to the top
    m_layout->addStretch();
    
    // Store button references for signal connections
    connect(btnNode, &QPushButton::clicked, this, &FloatingToolbar::nodeRequested);
    connect(btnRect, &QPushButton::clicked, this, &FloatingToolbar::rectangleRequested);
    connect(btnCircle, &QPushButton::clicked, this, &FloatingToolbar::circleRequested);
    connect(btnRed, &QPushButton::clicked, [this]() { emit fillColorChanged(Qt::red); });
    connect(btnBlue, &QPushButton::clicked, [this]() { emit fillColorChanged(Qt::blue); });
    connect(btnGreen, &QPushButton::clicked, [this]() { emit fillColorChanged(Qt::green); });
    connect(btnZoomIn, &QPushButton::clicked, this, &FloatingToolbar::zoomInRequested);
    connect(btnZoomOut, &QPushButton::clicked, this, &FloatingToolbar::zoomOutRequested);
    connect(btnResetView, &QPushButton::clicked, this, &FloatingToolbar::resetViewRequested);
}

void FloatingToolbar::connectSignals()
{
    // Connect toolbar signals to graphEditor slots
    if (m_graphEditor) {
        // Connect zoom controls
        connect(this, &FloatingToolbar::zoomInRequested, [this]() {
            m_graphEditor->scale(1.2, 1.2);
        });
        
        connect(this, &FloatingToolbar::zoomOutRequested, [this]() {
            m_graphEditor->scale(0.8, 0.8);
        });
        
        connect(this, &FloatingToolbar::resetViewRequested, [this]() {
            m_graphEditor->resetTransform();
        });
        
        // Node creation is handled in GraphEditorWindow
        // You'll need to add a slot in GraphEditorWindow to handle this
    }
}

void FloatingToolbar::positionRelativeToParent()
{
    if (m_graphEditor && m_graphEditor->isVisible()) {
        // Get the global position of the parent window
        QPoint parentPos = m_graphEditor->mapToGlobal(QPoint(0, 0));
        
        // Position the toolbar to the right of the main window with some padding
        int x = parentPos.x() + m_graphEditor->width() + 10;
        int y = parentPos.y();
        
        // Ensure the toolbar stays on screen
        QRect screenGeometry = QApplication::primaryScreen()->availableGeometry();
        if (x + width() > screenGeometry.right()) {
            // If toolbar would go off screen, position it to the left instead
            x = parentPos.x() - width() - 10;
        }
        
        move(x, y);
        
        qDebug() << "Toolbar repositioned to:" << QPoint(x, y);
    }
}

void FloatingToolbar::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    positionRelativeToParent();
}