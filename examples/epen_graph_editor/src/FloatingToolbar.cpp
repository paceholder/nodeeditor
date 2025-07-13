#include "FloatingToolbar.hpp"
#include "GraphEditorWindow.hpp"
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

FloatingToolbar::FloatingToolbar(GraphEditorWindow *parent)
    : QWidget(parent)
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

QString FloatingToolbar::createSafeButtonText(const QString &icon, const QString &text)
{
    // Check if system supports the icon by testing font metrics
    QFont testFont = QApplication::font();
    QFontMetrics fm(testFont);

    // Test if the icon character renders properly
    QRect boundingRect = fm.boundingRect(icon);

    // If the icon doesn't render properly (too small or empty), use fallback
    if (boundingRect.width() < 5 || boundingRect.height() < 5) {
        // Use fallback ASCII characters
        if (text.contains("Video Input"))
            return "< " + text;
        if (text.contains("Video Output"))
            return "> " + text;
        if (text.contains("Process"))
            return "* " + text;
        if (text.contains("Image"))
            return "# " + text;
        if (text.contains("Buffer"))
            return "= " + text;
        if (text.contains("Zoom In"))
            return "+ " + text;
        if (text.contains("Zoom Out"))
            return "- " + text;
        if (text.contains("Reset"))
            return "R " + text;
        return text;
    }

    return icon + " " + text;
}

QPushButton *FloatingToolbar::createRichTextButton(const QString &icon, const QString &text)
{
    QPushButton *button = new QPushButton();

    // For now, just use the safe text method
    button->setText(createSafeButtonText(icon, text));

    return button;
}

void FloatingToolbar::setupUI()
{
    // Create main widget with background
    QWidget *contentWidget = new QWidget(this);
    contentWidget->setObjectName("ToolbarContent");

    // Platform-specific font settings for better icon support
    QFont buttonFont = QApplication::font();
#ifdef Q_OS_MAC
    buttonFont.setFamily("Helvetica Neue");
#elif defined(Q_OS_WIN)
    buttonFont.setFamily("Segoe UI");
#endif
    buttonFont.setPointSize(11);

    contentWidget->setStyleSheet("#ToolbarContent {"
                                 "   background-color: #f5f5f5;"
                                 "   border: 1px solid #ccc;"
                                 "   border-radius: 6px;"
                                 "}"
                                 "QPushButton {"
                                 "   padding: 6px 8px;"
                                 "   margin: 2px;"
                                 "   border: 1px solid #bbb;"
                                 "   border-radius: 4px;"
                                 "   background-color: white;"
                                 "   text-align: left;"
                                 "   font-size: 11px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "   background-color: #e0e0e0;"
                                 "   border-color: #999;"
                                 "}"
                                 "QPushButton:pressed {"
                                 "   background-color: #d0d0d0;"
                                 "}"
                                 "QPushButton:disabled {"
                                 "   background-color: #f0f0f0;"
                                 "   color: #999;"
                                 "}");

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
    title->setStyleSheet("font-weight: bold;"
                         "padding: 8px;"
                         "background-color: #e0e0e0;"
                         "border-radius: 4px;"
                         "margin-bottom: 5px;");
    title->setCursor(Qt::SizeAllCursor);
    m_layout->addWidget(title);

    // Node creation section
    m_layout->addSpacing(5);
    QLabel *nodeLabel = new QLabel("Nodes:");
    nodeLabel->setStyleSheet("font-weight: bold; color: #333;");
    m_layout->addWidget(nodeLabel);

    // Define icons - using Unicode code points for safety
    struct NodeType
    {
        QString name;
        QString icon;
        QString fallback;
        QString tooltip;
        bool enabled;
    };

    QVector<NodeType> nodeTypes = {
        {"Video Input", QString::fromUtf8("\u25C0"), "<", "Create a video input node", true}, // ◀
        {"Video Output", QString::fromUtf8("\u25B6"), ">", "Create a video output node", false}, // ▶
        {"Process", QString::fromUtf8("\u2666"), "*", "Create a processing node", true}, // ♦
        {"Image", QString::fromUtf8("\u25A1"), "#", "Create an image node", true},       // □
        {"Buffer", QString::fromUtf8("\u25AC"), "=", "Create a buffer node", true}       // ▬
    };

    // Create buttons with proper icon handling
    for (const auto &nodeType : nodeTypes) {
        QPushButton *btn = new QPushButton();

        // Try to use the Unicode icon, fall back to ASCII if needed
        QString buttonText = createSafeButtonText(nodeType.icon, nodeType.name);
        btn->setText(buttonText);
        btn->setToolTip(nodeType.tooltip);
        btn->setEnabled(nodeType.enabled);
        btn->setFont(buttonFont);

        // Store the node type in a property for later use
        btn->setProperty("nodeType", nodeType.name);

        m_layout->addWidget(btn);

        // Connect signal
        connect(btn, &QPushButton::clicked, [this, nodeType]() {
            emit nodeRequested();
            // You can emit a specific signal with node type info if needed
            // emit specificNodeRequested(nodeType.name);
        });
    }

    // Separator
    m_layout->addSpacing(10);
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    m_layout->addWidget(separator);
    m_layout->addSpacing(5);

    // View controls section
    QLabel *viewLabel = new QLabel("View:");
    viewLabel->setStyleSheet("font-weight: bold; color: #333;");
    m_layout->addWidget(viewLabel);

    // View control buttons with safe icons
    struct ViewControl
    {
        QString name;
        QString icon;
        QString fallback;
    };

    QVector<ViewControl> viewControls = {
        {"Zoom In", QString::fromUtf8("\u2295"), "+"},   // ⊕
        {"Zoom Out", QString::fromUtf8("\u2296"), "-"},  // ⊖
        {"Reset View", QString::fromUtf8("\u21BA"), "R"} // ↺
    };

    QPushButton *btnZoomIn = new QPushButton(
        createSafeButtonText(viewControls[0].icon, viewControls[0].name));
    QPushButton *btnZoomOut = new QPushButton(
        createSafeButtonText(viewControls[1].icon, viewControls[1].name));
    QPushButton *btnResetView = new QPushButton(
        createSafeButtonText(viewControls[2].icon, viewControls[2].name));

    btnZoomIn->setFont(buttonFont);
    btnZoomOut->setFont(buttonFont);
    btnResetView->setFont(buttonFont);

    m_layout->addWidget(btnZoomIn);
    m_layout->addWidget(btnZoomOut);
    m_layout->addWidget(btnResetView);

    // Connect view control signals
    connect(btnZoomIn, &QPushButton::clicked, this, &FloatingToolbar::zoomInRequested);
    connect(btnZoomOut, &QPushButton::clicked, this, &FloatingToolbar::zoomOutRequested);
    connect(btnResetView, &QPushButton::clicked, this, &FloatingToolbar::resetViewRequested);

    // Add stretch to push everything to the top
    m_layout->addStretch();

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
    if (!m_graphEditor)
        return;

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
        QLabel *title = findChild<QLabel *>();
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

void FloatingToolbar::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updatePosition();
}