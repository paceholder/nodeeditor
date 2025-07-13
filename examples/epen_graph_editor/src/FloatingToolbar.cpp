#include "FloatingToolbar.hpp"
#include "GraphEditorWindow.hpp"
#include "QPushButton"
#include <QApplication>
#include <QDebug>
#include <QFontDatabase>
#include <QFontMetrics>
#include <QLabel>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QVBoxLayout>

FloatingToolbar::FloatingToolbar(GraphEditorWindow *parent)
    : QWidget(parent) // Always a child widget
    , m_graphEditor(parent)
    , m_dragging(false)
    , m_dockPosition(Floating)
    , m_previewDockPosition(Floating)
    , m_dockMargin(0) // No margin when docked
    , m_dockingDistance(40)
    , m_dockedWidth(200) // Wider when docked
{
    // Keep it as a child widget with these flags
    setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Create animation for smooth transitions
    m_geometryAnimation = new QPropertyAnimation(this, "geometry");
    m_geometryAnimation->setDuration(200);
    m_geometryAnimation->setEasingCurve(QEasingCurve::OutCubic);

    setupUI();
    connectSignals();

    // Initial floating size and position
    setFixedWidth(150);
    if (parent) {
        m_floatingGeometry = QRect((parent->width() - 150) / 2,
                                   (parent->height() - height()) / 2,
                                   150,
                                   height());
        setGeometry(m_floatingGeometry);
    }

    // Ensure toolbar is on top
    raise();
    _floatHeight = height();
}

QString FloatingToolbar::createSafeButtonText(const QString &icon, const QString &text)
{
    QFont testFont = QApplication::font();
    QFontMetrics fm(testFont);
    QRect boundingRect = fm.boundingRect(icon);

    if (boundingRect.width() < 5 || boundingRect.height() < 5) {
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

void FloatingToolbar::setupUI()
{
    // Create scroll area for when docked
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_contentWidget = new QWidget();
    m_contentWidget->setObjectName("ToolbarContent");

    QFont buttonFont = QApplication::font();
#ifdef Q_OS_MAC
    buttonFont.setFamily("Helvetica Neue");
#elif defined(Q_OS_WIN)
    buttonFont.setFamily("Segoe UI");
#endif
    buttonFont.setPointSize(11);

    m_scrollArea->setStyleSheet("QScrollArea {"
                                "   background: transparent;"
                                "   border: none;"
                                "}"
                                "QScrollBar:vertical {"
                                "   background: #f0f0f0;"
                                "   width: 10px;"
                                "   border-radius: 5px;"
                                "}"
                                "QScrollBar::handle:vertical {"
                                "   background: #c0c0c0;"
                                "   border-radius: 5px;"
                                "   min-height: 20px;"
                                "}"
                                "QScrollBar::handle:vertical:hover {"
                                "   background: #a0a0a0;"
                                "}");

    m_contentWidget->setStyleSheet("#ToolbarContent {"
                                   "   background-color: #f5f5f5;"
                                   "   border: 1px solid #ccc;"
                                   "   border-radius: 6px;"
                                   "}"
                                   "DraggableButton {"
                                   "   padding: 6px 8px;"
                                   "   margin: 2px;"
                                   "   border: 1px solid #bbb;"
                                   "   border-radius: 4px;"
                                   "   background-color: white;"
                                   "   text-align: left;"
                                   "   font-size: 11px;"
                                   "}"
                                   "DraggableButton:hover {"
                                   "   background-color: #e0e0e0;"
                                   "   border-color: #999;"
                                   "}"
                                   "DraggableButton:pressed {"
                                   "   background-color: #d0d0d0;"
                                   "}"
                                   "DraggableButton:disabled {"
                                   "   background-color: #f0f0f0;"
                                   "   color: #999;"
                                   "}");

    // Set scroll area content
    m_scrollArea->setWidget(m_contentWidget);

    // Main layout for this widget
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(m_scrollArea);

    // Content layout
    m_layout = new QVBoxLayout(m_contentWidget);
    m_layout->setContentsMargins(8, 8, 8, 8);
    m_layout->setSpacing(4);

    // Title bar for dragging
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

    struct NodeType
    {
        QString name;
        QString icon;
        QString fallback;
        QString tooltip;
        bool enabled;
        QString actionName;
    };

    QVector<NodeType> nodeTypes = {
        {"Video Input", QString::fromUtf8("\u25C0"), "<", "Create a video input node", true, "input"},
        {"Video Output",
         QString::fromUtf8("\u25B6"),
         ">",
         "Create a video output node",
         false,
         "output"},
        {"Process", QString::fromUtf8("\u2666"), "*", "Create a processing node", true, "process"},
        {"Image", QString::fromUtf8("\u25A1"), "#", "Create an image node", true, "image"},
        {"Buffer", QString::fromUtf8("\u25AC"), "=", "Create a buffer node", true, "buffer"}};

    for (const auto &nodeType : nodeTypes) {
        DraggableButton *btn = new DraggableButton(nodeType.actionName, this);
        QString buttonText = createSafeButtonText(nodeType.icon, nodeType.name);
        btn->setText(buttonText);
        btn->setToolTip(nodeType.tooltip);
        btn->setEnabled(nodeType.enabled);
        btn->setCheckable(true);
        btn->setChecked(true);
        btn->setFont(buttonFont);
        btn->setProperty("nodeType", nodeType.name);
        m_layout->addWidget(btn);
    }

    // Separator
    m_layout->addSpacing(10);
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    m_layout->addWidget(separator);
    m_layout->addSpacing(5);

    // View controls
    QLabel *viewLabel = new QLabel("View:");
    viewLabel->setStyleSheet("font-weight: bold; color: #333;");
    m_layout->addWidget(viewLabel);

    struct ViewControl
    {
        QString name;
        QString icon;
        QString fallback;
    };

    QVector<ViewControl> viewControls = {{"Zoom In", QString::fromUtf8("\u2295"), "+"},
                                         {"Zoom Out", QString::fromUtf8("\u2296"), "-"},
                                         {"Reset View", QString::fromUtf8("\u21BA"), "R"}};

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

    connect(btnZoomIn, &QPushButton::clicked, this, &FloatingToolbar::zoomInRequested);
    connect(btnZoomOut, &QPushButton::clicked, this, &FloatingToolbar::zoomOutRequested);
    connect(btnResetView, &QPushButton::clicked, this, &FloatingToolbar::resetViewRequested);

    m_layout->addStretch();

    // Initial size
    m_contentWidget->adjustSize();
    adjustSize();
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
    if (m_dockPosition != Floating) {
        updateDockedGeometry();
    }
}

void FloatingToolbar::setDockPosition(DockPosition position)
{
    if (m_dockPosition == position)
        return;

    m_dockPosition = position;

    if (position == Floating) {
        // Restore floating geometry
        setMinimumSize(0, 0);
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        setFixedWidth(150);

        if (m_geometryAnimation->state() == QAbstractAnimation::Running) {
            m_geometryAnimation->stop();
        }
        m_geometryAnimation->setStartValue(geometry());
        m_geometryAnimation->setEndValue(m_floatingGeometry);
        m_geometryAnimation->start();
    } else {
        // Apply docked geometry
        updateDockedGeometry();
    }
}

FloatingToolbar::DockPosition FloatingToolbar::checkDockingZone(const QPoint &pos)
{
    if (!parentWidget())
        return Floating;

    int parentWidth = parentWidget()->width();

    // Check left edge
    if (pos.x() <= m_dockingDistance) {
        return DockedLeft;
    }

    // Check right edge
    if (pos.x() + width() >= parentWidth - m_dockingDistance) {
        return DockedRight;
    }

    return Floating;
}

void FloatingToolbar::applyDocking(DockPosition position)
{
    setDockPosition(position);
}

void FloatingToolbar::updateDockedGeometry()
{
    if (!parentWidget() || m_dockPosition == Floating) {
        return;
    }

    QRect targetGeometry;
    int parentHeight = parentWidget()->height();

    // Remove size constraints for docking
    setMinimumSize(0, 0);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    switch (m_dockPosition) {
    case DockedLeft:
        targetGeometry = QRect(m_dockMargin,
                               m_dockMargin,
                               m_dockedWidth,
                               parentHeight - 2 * m_dockMargin);
        break;
    case DockedRight:
        targetGeometry = QRect(parentWidget()->width() - m_dockedWidth - m_dockMargin,
                               m_dockMargin,
                               m_dockedWidth,
                               parentHeight - 2 * m_dockMargin);
        break;
    default:
        return;
    }

    if (m_geometryAnimation->state() == QAbstractAnimation::Running) {
        m_geometryAnimation->stop();
    }
    m_geometryAnimation->setStartValue(geometry());
    m_geometryAnimation->setEndValue(targetGeometry);
    m_geometryAnimation->start();

    m_contentWidget->setStyleSheet("#ToolbarContent {"
                                   "   background-color: #f5f5f5;"
                                   "   border: 1px solid #ccc;"
                                   "   border-radius: 0px;"
                                   "}");
}

void FloatingToolbar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw shadow
    QRect shadowRect = rect().adjusted(4, 4, -4, -4);
    painter.fillRect(shadowRect, QColor(0, 0, 0, 30));

    // Draw docking preview
    if (m_dragging && m_previewDockPosition != Floating && m_previewDockPosition != m_dockPosition) {
        painter.setPen(QPen(QColor(0, 120, 215), 2));
        painter.setBrush(QColor(0, 120, 215, 20));
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);
    }

    QWidget::paintEvent(event);
}

void FloatingToolbar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        QLabel *title = findChild<QLabel *>();
        if (title && title->geometry().contains(event->pos())) {
            m_dragging = true;
            m_dragStartPosition = event->pos();

            // Store current geometry if floating
            if (m_dockPosition == Floating) {
                m_floatingGeometry = geometry();
            }

            raise(); // Bring to front when dragging
        }
    }
    QWidget::mousePressEvent(event);
}

void FloatingToolbar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = pos() + event->pos() - m_dragStartPosition;

        // If currently docked, undock first
        if (m_dockPosition != Floating) {
            m_dockPosition = Floating;
            setFixedWidth(150);
            // Adjust position to keep mouse on title bar
            newPos = QPoint(event->globalPos().x() - m_dragStartPosition.x(),
                            event->globalPos().y() - m_dragStartPosition.y());
            if (parentWidget()) {
                newPos = parentWidget()->mapFromGlobal(newPos);
            }
            setFixedHeight(_floatHeight);
            m_contentWidget->setStyleSheet("#ToolbarContent {"
                                           "   background-color: #f5f5f5;"
                                           "   border: 1px solid #ccc;"
                                           "   border-radius: 6px;"
                                           "}");
        }

        // Keep within parent bounds
        if (parentWidget()) {
            int maxX = parentWidget()->width() - width();
            int maxY = parentWidget()->height() - height();
            newPos.setX(qMax(0, qMin(newPos.x(), maxX)));
            newPos.setY(qMax(0, qMin(newPos.y(), maxY)));
        }

        move(newPos);

        // Check for docking zones
        m_previewDockPosition = checkDockingZone(newPos);

        // Remember floating position
        if (m_previewDockPosition == Floating) {
            m_floatingGeometry = QRect(newPos, size());
        }

        update(); // Repaint for preview
    }
    QWidget::mouseMoveEvent(event);
}

void FloatingToolbar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_dragging) {
        m_dragging = false;

        // Apply docking if in zone
        if (m_previewDockPosition != Floating) {
            applyDocking(m_previewDockPosition);
        }

        m_previewDockPosition = Floating;
        update();
    }
    QWidget::mouseReleaseEvent(event);
}

void FloatingToolbar::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updatePosition();
    raise(); // Ensure toolbar is on top
}

void FloatingToolbar::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // If docked, maintain the docked state
    if (m_dockPosition != Floating) {
        updateDockedGeometry();
    }
}