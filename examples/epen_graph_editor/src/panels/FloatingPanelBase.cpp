#include "panels/FloatingPanelBase.hpp"
#include "GraphEditorMainWindow.hpp"
#include <QApplication>
#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QVBoxLayout>

FloatingPanelBase::FloatingPanelBase(GraphEditorWindow *parent, const QString &title)
    : QWidget(parent)
    , m_graphEditor(parent)
    , m_dragging(false)
    , m_dockPosition(Floating)
    , m_previewDockPosition(Floating)
    , m_dockMargin(0)
    , m_dockingDistance(40)
    , m_dockedWidth(200)
    , m_dockedHeight(300)  // Initialize default docked height
    , m_floatingWidth(150)
    , m_panelTitle(title)
{
    // Keep it as a child widget with these flags
    setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);

    // Create animation for smooth transitions
    m_geometryAnimation = new QPropertyAnimation(this, "geometry");
    m_geometryAnimation->setDuration(200);
    m_geometryAnimation->setEasingCurve(QEasingCurve::OutCubic);
}

FloatingPanelBase::~FloatingPanelBase() {}

void FloatingPanelBase::setupBaseUI(const QString &title)
{
    // Create scroll area for when docked
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_contentWidget = new QWidget();
    m_contentWidget->setObjectName(m_panelTitle.replace(" ", "") + "Content");

    // Apply panel-specific style
    m_scrollArea->setStyleSheet(getPanelStyleSheet());
    m_contentWidget->setStyleSheet(getContentStyleSheet());

    // Set scroll area content
    m_scrollArea->setWidget(m_contentWidget);

    // Main layout for this widget
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->addWidget(m_scrollArea);

    // Content layout
    m_contentLayout = new QVBoxLayout(m_contentWidget);
    m_contentLayout->setContentsMargins(8, 8, 8, 8);
    m_contentLayout->setSpacing(4);

    // Title bar for dragging
    m_titleLabel = new QLabel(title);
    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet("font-weight: bold;"
                                "padding: 8px;"
                                "background-color: #e0e0e0;"
                                "border-radius: 4px;"
                                "margin-bottom: 5px;");
    m_titleLabel->setCursor(Qt::SizeAllCursor);
    m_contentLayout->addWidget(m_titleLabel);
}

QString FloatingPanelBase::getPanelStyleSheet() const
{
    return "QScrollArea {"
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
           "}";
}

QString FloatingPanelBase::getContentStyleSheet() const
{
    QString title = m_panelTitle;
    QString replacedTitle = title.replace(" ", "");
    QString objectName = replacedTitle + "Content";
    return QString("#%1 {"
                   "   background-color: #f5f5f5;"
                   "   border: 1px solid #ccc;"
                   "   border-radius: 6px;"
                   "}")
        .arg(objectName);
}

void FloatingPanelBase::updatePosition()
{
    if (m_dockPosition != Floating) {
        updateDockedGeometry();
    }
}

void FloatingPanelBase::setDockPosition(DockPosition position)
{
    if (m_dockPosition == position)
        return;

    m_dockPosition = position;

    if (position == Floating) {
        // Restore floating geometry
        setMinimumSize(0, 0);
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        setFixedWidth(m_floatingWidth);

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

FloatingPanelBase::DockPosition FloatingPanelBase::checkDockingZone(const QPoint &pos)
{
    if (!parentWidget())
        return Floating;

    int parentWidth = parentWidget()->width();
    int parentHeight = parentWidget()->height();

    // Check left edge
    if (pos.x() <= m_dockingDistance) {
        return DockedLeft;
    }

    // Check right edge
    if (pos.x() + width() >= parentWidth - m_dockingDistance) {
        return DockedRight;
    }

    // Check bottom edge
    if (pos.y() + height() >= parentHeight - m_dockingDistance) {
        return DockedBottom;
    }

    return Floating;
}

void FloatingPanelBase::applyDocking(DockPosition position)
{
    setDockPosition(position);
}

void FloatingPanelBase::updateDockedGeometry()
{
    if (!parentWidget() || m_dockPosition == Floating) {
        return;
    }

    QRect targetGeometry;
    int parentWidth = parentWidget()->width();
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
    case DockedBottom:
        targetGeometry = QRect(m_dockMargin,
                               parentHeight - m_dockedHeight - m_dockMargin,
                               parentWidth - 2 * m_dockMargin,
                               m_dockedHeight);
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

    // Update style for docked state (remove border radius)
    QString objectName = m_panelTitle.replace(" ", "") + "Content";
    m_contentWidget->setStyleSheet(QString("#%1 {"
                                           "   background-color: #f5f5f5;"
                                           "   border: 1px solid #ccc;"
                                           "   border-radius: 0px;"
                                           "}")
                                       .arg(objectName));
}

void FloatingPanelBase::paintEvent(QPaintEvent *event)
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

void FloatingPanelBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_titleLabel && m_titleLabel->geometry().contains(event->pos())) {
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

void FloatingPanelBase::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = pos() + event->pos() - m_dragStartPosition;

        // If currently docked, undock first
        if (m_dockPosition != Floating) {
            m_dockPosition = Floating;
            setFixedWidth(m_floatingWidth);
            // Adjust position to keep mouse on title bar
            newPos = QPoint(event->globalPosition().x() - m_dragStartPosition.x(),
                            event->globalPosition().y() - m_dragStartPosition.y());
            if (parentWidget()) {
                newPos = parentWidget()->mapFromGlobal(newPos);
            }
            setFixedHeight(m_floatHeight);
            m_contentWidget->setStyleSheet(getContentStyleSheet());
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

void FloatingPanelBase::mouseReleaseEvent(QMouseEvent *event)
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

void FloatingPanelBase::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updatePosition();
    raise(); // Ensure panel is on top
}

void FloatingPanelBase::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // If docked, maintain the docked state
    if (m_dockPosition != Floating) {
        updateDockedGeometry();
    }
}