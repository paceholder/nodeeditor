#include "FloatingToolbar.hpp"
#include "GraphEditorWindow.hpp"
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPaintEvent>
#include <QPainter>
#include <QMouseEvent>
#include <QApplication>
#include <QDebug>

FloatingToolbar::FloatingToolbar(GraphEditorWindow *parent)
    : QWidget(parent)  // Parent is set, making this an embedded widget
    , m_graphEditor(parent)
    , m_dragging(false)
    , m_dockedRight(true)
    , m_margin(10)
{
    setWindowTitle("Tools");
    setFixedWidth(150);
    
    // Make it stay on top within the parent widget
    setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    setupUI();
    connectSignals();
    
    // Initial position
    updatePosition();
}

void FloatingToolbar::setupUI()
{
    // Create main widget with background
    QWidget *contentWidget = new QWidget(this);
    contentWidget->setObjectName("ToolbarContent");
    contentWidget->setStyleSheet(
        "#ToolbarContent {"
        "   background-color: #f5f5f5;"
        "   border: 1px solid #ccc;"
        "   border-radius: 6px;"
        "}"
        "QPushButton {"
        "   padding: 5px;"
        "   margin: 2px;"
        "   border: 1px solid #bbb;"
        "   border-radius: 4px;"
        "   background-color: white;"
        "}"
        "QPushButton:hover {"
        "   background-color: #e0e0e0;"
        "   border-color: #999;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #d0d0d0;"
        "}"
    );
    
    // Main layout for the widget
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(contentWidget);
    
    // Content layout
    m_layout = new QVBoxLayout(contentWidget);
    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->setSpacing(4);
    
    // Add title (acts as drag handle)
    QLabel *title = new QLabel("Tools");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet(
        "font-weight: bold;"
        "padding: 8px;"
        "background-color: #e0e0e0;"
        "border-radius: 4px;"
        "margin-bottom: 5px;"
    );
    title->setCursor(Qt::SizeAllCursor);
    m_layout->addWidget(title);
    
    // Node creation section
    m_layout->addSpacing(5);
    QLabel *nodeLabel = new QLabel("Nodes:");
    nodeLabel->setStyleSheet("font-weight: bold; color: #333;");
    m_layout->addWidget(nodeLabel);
    
    QPushButton *btnNode = new QPushButton("Create Node");
    btnNode->setToolTip("Create a new node at cursor position");
    m_layout->addWidget(btnNode);
    
    // Shape tools section
    m_layout->addSpacing(10);
    QLabel *shapeLabel = new QLabel("Shapes:");
    shapeLabel->setStyleSheet("font-weight: bold; color: #333;");
    m_layout->addWidget(shapeLabel);
    
    QPushButton *btnRect = new QPushButton("Rectangle");
    btnRect->setEnabled(false);
    m_layout->addWidget(btnRect);
    
    QPushButton *btnCircle = new QPushButton("Circle");
    btnCircle->setEnabled(false);
    m_layout->addWidget(btnCircle);
    
    // Color section
    m_layout->addSpacing(10);
    QLabel *colorLabel = new QLabel("Colors:");
    colorLabel->setStyleSheet("font-weight: bold; color: #333;");
    m_layout->addWidget(colorLabel);
    
    QPushButton *btnRed = new QPushButton("Red Fill");
    btnRed->setStyleSheet(btnRed->styleSheet() + "QPushButton { background-color: #ffdddd; }");
    m_layout->addWidget(btnRed);
    
    QPushButton *btnBlue = new QPushButton("Blue Fill");
    btnBlue->setStyleSheet(btnBlue->styleSheet() + "QPushButton { background-color: #ddddff; }");
    m_layout->addWidget(btnBlue);
    
    QPushButton *btnGreen = new QPushButton("Green Fill");
    btnGreen->setStyleSheet(btnGreen->styleSheet() + "QPushButton { background-color: #ddffdd; }");
    m_layout->addWidget(btnGreen);
    
    // View controls section
    m_layout->addSpacing(10);
    QLabel *viewLabel = new QLabel("View:");
    viewLabel->setStyleSheet("font-weight: bold; color: #333;");
    m_layout->addWidget(viewLabel);
    
    QPushButton *btnZoomIn = new QPushButton("Zoom In");
    m_layout->addWidget(btnZoomIn);
    
    QPushButton *btnZoomOut = new QPushButton("Zoom Out");
    m_layout->addWidget(btnZoomOut);
    
    QPushButton *btnResetView = new QPushButton("Reset View");
    m_layout->addWidget(btnResetView);
    
    // Add stretch to push everything to the top
    m_layout->addStretch();
    
    // Connect signals
    connect(btnNode, &QPushButton::clicked, this, &FloatingToolbar::nodeRequested);
    connect(btnRect, &QPushButton::clicked, this, &FloatingToolbar::rectangleRequested);
    connect(btnCircle, &QPushButton::clicked, this, &FloatingToolbar::circleRequested);
    connect(btnRed, &QPushButton::clicked, [this]() { emit fillColorChanged(Qt::red); });
    connect(btnBlue, &QPushButton::clicked, [this]() { emit fillColorChanged(Qt::blue); });
    connect(btnGreen, &QPushButton::clicked, [this]() { emit fillColorChanged(Qt::green); });
    connect(btnZoomIn, &QPushButton::clicked, this, &FloatingToolbar::zoomInRequested);
    connect(btnZoomOut, &QPushButton::clicked, this, &FloatingToolbar::zoomOutRequested);
    connect(btnResetView, &QPushButton::clicked, this, &FloatingToolbar::resetViewRequested);
    
    // Calculate height based on content
    adjustSize();
    setFixedHeight(sizeHint().height());
}

void FloatingToolbar::connectSignals()
{
    if (m_graphEditor) {
        connect(this, &FloatingToolbar::zoomInRequested, [this]() {
            m_graphEditor->scale(1.2, 1.2);
        });
        
        connect(this, &FloatingToolbar::zoomOutRequested, [this]() {
            m_graphEditor->scale(0.8, 0.8);
        });
        
        connect(this, &FloatingToolbar::resetViewRequested, [this]() {
            m_graphEditor->resetTransform();
        });
    }
}

void FloatingToolbar::updatePosition()
{
    if (!m_graphEditor) return;
    
    int x, y;
    
    if (m_dockedRight) {
        x = m_graphEditor->width() - width() - m_margin;
    } else {
        x = m_margin;
    }
    
    y = m_margin;
    
    // Ensure toolbar stays within bounds
    x = qMax(m_margin, qMin(x, m_graphEditor->width() - width() - m_margin));
    y = qMax(m_margin, qMin(y, m_graphEditor->height() - height() - m_margin));
    
    move(x, y);
}

void FloatingToolbar::setDockedRight(bool right)
{
    m_dockedRight = right;
    updatePosition();
}

void FloatingToolbar::paintEvent(QPaintEvent *event)
{
    // Draw shadow
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    // Shadow
    QRect shadowRect = rect().adjusted(4, 4, -4, -4);
    painter.fillRect(shadowRect, QColor(0, 0, 0, 30));
    
    QWidget::paintEvent(event);
}

void FloatingToolbar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QLabel *title = findChild<QLabel*>();
        if (title && title->geometry().contains(event->pos())) {
            m_dragging = true;
            m_dragStartPosition = event->pos();
        }
    }
    QWidget::mousePressEvent(event);
}

void FloatingToolbar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = pos() + event->pos() - m_dragStartPosition;
        
        // Keep toolbar within parent bounds
        int maxX = parentWidget()->width() - width() - m_margin;
        int maxY = parentWidget()->height() - height() - m_margin;
        
        newPos.setX(qMax(m_margin, qMin(newPos.x(), maxX)));
        newPos.setY(qMax(m_margin, qMin(newPos.y(), maxY)));
        
        move(newPos);
        
        // Update docked state based on position
        if (newPos.x() < parentWidget()->width() / 2) {
            m_dockedRight = false;
        } else {
            m_dockedRight = true;
        }
    }
    QWidget::mouseMoveEvent(event);
}

void FloatingToolbar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}