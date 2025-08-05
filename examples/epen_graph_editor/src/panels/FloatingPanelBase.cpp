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
    , m_dockedHeight(300) // Initialize default docked height
    , m_floatingWidth(150)
    , m_priority(1) // Default priority
    , m_panelTitle(title)
    , m_isResizable(false) // Default to non-resizable
    , m_resizing(false)
    , m_resizeEdge(NoEdge)
    , m_resizeStartHeight(0)
{
    // Keep it as a child widget with these flags
    setWindowFlags(Qt::SubWindow | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    
    // Enable mouse tracking for resize cursor
    setMouseTracking(true);

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
    m_titleLabel->setMouseTracking(true); // Enable mouse tracking on title
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
           "}"
           "QScrollBar::add-line:vertical,"
           "QScrollBar::sub-line:vertical {"
           "   height: 0px;"
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

    DockPosition oldPosition = m_dockPosition;
    m_dockPosition = position;

    if (position == Floating) {
        // Restore floating geometry
        setMinimumSize(150, 100);  // Minimum size for floating
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        
        // Don't use setFixedWidth for floating panels - it prevents resizing!
        resize(m_floatingWidth, m_floatHeight);

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

    // Emit signal after position change
    emit dockPositionChanged(position);

    // Update all panels if we're docking/undocking from bottom
    if (oldPosition == DockedBottom || position == DockedBottom) {
        updateAllDockedPanels();
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

int FloatingPanelBase::getBottomPanelHeight() const
{
    if (!m_graphEditor)
        return 0;

    // Find bottom-docked panel with highest priority
    FloatingPanelBase *bottomPanel = nullptr;
    int maxPriority = -1;

    for (QObject *obj : m_graphEditor->children()) {
        if (FloatingPanelBase *panel = qobject_cast<FloatingPanelBase *>(obj)) {
            if (panel->dockPosition() == DockedBottom && panel->panelPriority() > maxPriority
                && panel->isVisible()) {
                bottomPanel = panel;
                maxPriority = panel->panelPriority();
            }
        }
    }

    return bottomPanel ? bottomPanel->dockedHeight() : 0;
}

QRect FloatingPanelBase::calculateDockedGeometry(DockPosition position)
{
    if (!parentWidget())
        return QRect();

    int parentWidth = parentWidget()->width();
    int parentHeight = parentWidget()->height();
    int bottomPanelHeight = 0;

    // Check if there's a bottom panel (only for side panels)
    if (position != DockedBottom) {
        bottomPanelHeight = getBottomPanelHeight();
    }

    switch (position) {
    case DockedLeft:
        return QRect(m_dockMargin,
                     m_dockMargin,
                     m_dockedWidth,
                     parentHeight - 2 * m_dockMargin - bottomPanelHeight);

    case DockedRight:
        return QRect(parentWidth - m_dockedWidth - m_dockMargin,
                     m_dockMargin,
                     m_dockedWidth,
                     parentHeight - 2 * m_dockMargin - bottomPanelHeight);

    case DockedBottom:
        return QRect(m_dockMargin,
                     parentHeight - m_dockedHeight - m_dockMargin,
                     parentWidth - 2 * m_dockMargin,
                     m_dockedHeight);

    default:
        return QRect();
    }
}

void FloatingPanelBase::updateDockedGeometry()
{
    if (!parentWidget() || m_dockPosition == Floating) {
        return;
    }

    // Remove size constraints for docking
    setMinimumSize(0, 0);
    setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);

    QRect targetGeometry = calculateDockedGeometry(m_dockPosition);

    if (m_geometryAnimation->state() == QAbstractAnimation::Running) {
        m_geometryAnimation->stop();
    }
    
    // For bottom docked panels, disable animation during resize for smoother experience
    if (m_dockPosition == DockedBottom && m_resizing) {
        setGeometry(targetGeometry);
    } else {
        m_geometryAnimation->setStartValue(geometry());
        m_geometryAnimation->setEndValue(targetGeometry);
        m_geometryAnimation->start();
    }

    // Update style for docked state (remove border radius)
    QString objectName = m_panelTitle.replace(" ", "") + "Content";
    m_contentWidget->setStyleSheet(QString("#%1 {"
                                           "   background-color: #f5f5f5;"
                                           "   border: 1px solid #ccc;"
                                           "   border-radius: 0px;"
                                           "}")
                                       .arg(objectName));
}

void FloatingPanelBase::updateAllDockedPanels()
{
    if (!m_graphEditor)
        return;

    // Update all docked panels to adjust for bottom panel
    for (QObject *obj : m_graphEditor->children()) {
        if (FloatingPanelBase *panel = qobject_cast<FloatingPanelBase *>(obj)) {
            if (panel != this && panel->isDocked()) {
                panel->updatePosition();
            }
        }
    }
    dockChanged(m_dockPosition == Floating);
}

void FloatingPanelBase::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw shadow only when floating
    if (m_dockPosition == Floating) {
        QRect shadowRect = rect().adjusted(4, 4, -4, -4);
        painter.fillRect(shadowRect, QColor(0, 0, 0, 30));
    }

    // Draw docking preview
    if (m_dragging && m_previewDockPosition != Floating && m_previewDockPosition != m_dockPosition) {
        painter.setPen(QPen(QColor(0, 120, 215), 2));
        painter.setBrush(QColor(0, 120, 215, 20));
        painter.drawRoundedRect(rect().adjusted(1, 1, -1, -1), 6, 6);
    }
    
    // Draw resize grip for docked bottom mode - only for resizable panels
    if (m_isResizable && m_dockPosition == DockedBottom) {
        // Draw resize handle at top edge
        int handleHeight = 4;
        QRect handleRect(0, 0, width(), handleHeight);
        painter.fillRect(handleRect, QColor(200, 200, 200, 100));
        
        // Draw grip lines
        painter.setPen(QPen(QColor(150, 150, 150), 1));
        int centerX = width() / 2;
        int lineWidth = 30;
        for (int i = -1; i <= 1; i++) {
            int y = 2 + i;
            painter.drawLine(centerX - lineWidth/2, y, centerX + lineWidth/2, y);
        }
    }

    QWidget::paintEvent(event);
}

void FloatingPanelBase::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        // Check for resize first - only if panel is resizable
        if (m_isResizable && (m_dockPosition == Floating || m_dockPosition == DockedBottom)) {
            m_resizeEdge = getResizeEdge(event->pos());
            if (m_resizeEdge != NoEdge) {
                m_resizing = true;
                m_resizeStartPos = event->globalPosition().toPoint();
                m_resizeStartGeometry = geometry();
                if (m_dockPosition == DockedBottom) {
                    m_resizeStartHeight = m_dockedHeight;
                }
                event->accept();
                return; // Important: return here to prevent dragging
            }
        }
        
        // Only allow dragging from the title bar, not from anywhere else
        if (m_titleLabel && m_titleLabel->geometry().contains(event->pos())) {
            m_dragging = true;
            m_dragStartPosition = event->pos();

            // Store current geometry if floating
            if (m_dockPosition == Floating) {
                m_floatingGeometry = geometry();
            }

            raise(); // Bring to front when dragging
            event->accept();
            return;
        }
    }
    QWidget::mousePressEvent(event);
}

void FloatingPanelBase::mouseMoveEvent(QMouseEvent *event)
{
    if (m_resizing && (event->buttons() & Qt::LeftButton)) {
        // Handle resizing
        QPoint delta = event->globalPosition().toPoint() - m_resizeStartPos;
        
        if (m_dockPosition == DockedBottom) {
            // For bottom docked panels, only allow vertical resize from top edge
            int newHeight = qMax(100, qMin(parentWidget()->height() - 100, m_resizeStartHeight - delta.y()));
            if (newHeight != m_dockedHeight) {
                m_dockedHeight = newHeight;
                updateDockedGeometry();
                updateAllDockedPanels(); // Update other panels when resizing
            }
        } else if (m_dockPosition == Floating) {
            // For floating panels, allow resize from all edges
            QRect newGeometry = m_resizeStartGeometry;
            
            switch (m_resizeEdge) {
                case TopEdge:
                    newGeometry.setTop(m_resizeStartGeometry.top() + delta.y());
                    break;
                case BottomEdge:
                    newGeometry.setBottom(m_resizeStartGeometry.bottom() + delta.y());
                    break;
                case LeftEdge:
                    newGeometry.setLeft(m_resizeStartGeometry.left() + delta.x());
                    break;
                case RightEdge:
                    newGeometry.setRight(m_resizeStartGeometry.right() + delta.x());
                    break;
                case TopLeftCorner:
                    newGeometry.setTopLeft(m_resizeStartGeometry.topLeft() + delta);
                    break;
                case TopRightCorner:
                    newGeometry.setTop(m_resizeStartGeometry.top() + delta.y());
                    newGeometry.setRight(m_resizeStartGeometry.right() + delta.x());
                    break;
                case BottomLeftCorner:
                    newGeometry.setBottom(m_resizeStartGeometry.bottom() + delta.y());
                    newGeometry.setLeft(m_resizeStartGeometry.left() + delta.x());
                    break;
                case BottomRightCorner:
                    newGeometry.setBottomRight(m_resizeStartGeometry.bottomRight() + delta);
                    break;
                default:
                    break;
            }
            
            // Apply minimum size constraints
            if (newGeometry.width() >= 150 && newGeometry.height() >= 100) {
                setGeometry(newGeometry);
                m_floatingGeometry = newGeometry;
                m_floatHeight = newGeometry.height();
                m_floatingWidth = newGeometry.width();
                updateAllDockedPanels(); // Update other panels when resizing
            }
        }
        
        event->accept();
        return;
    } else if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = pos() + event->pos() - m_dragStartPosition;

        // If currently docked, undock first
        if (m_dockPosition != Floating) {
            DockPosition oldPosition = m_dockPosition;
            m_dockPosition = Floating;
            
            // Don't use setFixedWidth - it prevents resizing!
            resize(m_floatingWidth, m_floatHeight);
            
            // Adjust position to keep mouse on title bar
            newPos = QPoint(event->globalPosition().x() - m_dragStartPosition.x(),
                            event->globalPosition().y() - m_dragStartPosition.y());
            if (parentWidget()) {
                newPos = parentWidget()->mapFromGlobal(newPos);
            }
            
            m_contentWidget->setStyleSheet(getContentStyleSheet());

            // Update other panels if we were docked at bottom
            if (oldPosition == DockedBottom) {
                updateAllDockedPanels();
            }
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
    } else {
        // Always update cursor based on position when not actively doing something
        if (!m_dragging && !m_resizing) {
            updateCursor(event->pos());
        }
    }
    
    QWidget::mouseMoveEvent(event);
}

void FloatingPanelBase::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (m_resizing) {
            m_resizing = false;
            m_resizeEdge = NoEdge;
            // Don't set cursor here, let mouseMoveEvent handle it
            updateCursor(event->pos());
        } else if (m_dragging) {
            m_dragging = false;

            // Apply docking if in zone
            if (m_previewDockPosition != Floating) {
                applyDocking(m_previewDockPosition);
            }

            m_previewDockPosition = Floating;
            update();
            
            // Update cursor after dragging ends
            updateCursor(event->pos());
        }
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

void FloatingPanelBase::dockChanged(bool isFloat) {}

void FloatingPanelBase::enterEvent(QEnterEvent *event)
{
    // Update cursor when mouse enters the widget
    if (m_isResizable) {
        updateCursor(mapFromGlobal(QCursor::pos()));
    }
    QWidget::enterEvent(event);
}

void FloatingPanelBase::leaveEvent(QEvent *event)
{
    // Reset cursor when mouse leaves the widget
    if (!m_resizing && !m_dragging) {
        setCursor(Qt::ArrowCursor);
    }
    QWidget::leaveEvent(event);
}

FloatingPanelBase::ResizeEdge FloatingPanelBase::getResizeEdge(const QPoint &pos)
{
    if (m_dockPosition == DockedBottom) {
        // For bottom docked panels, only allow resize from top edge
        if (pos.y() < RESIZE_MARGIN) {
            return TopEdge;
        }
        return NoEdge;
    } else if (m_dockPosition == Floating) {
        // For floating panels, allow resize from all edges
        QRect r = rect();
        bool onLeft = pos.x() < RESIZE_MARGIN;
        bool onRight = pos.x() > r.width() - RESIZE_MARGIN;
        bool onTop = pos.y() < RESIZE_MARGIN;
        bool onBottom = pos.y() > r.height() - RESIZE_MARGIN;
        
        if (onTop && onLeft) return TopLeftCorner;
        if (onTop && onRight) return TopRightCorner;
        if (onBottom && onLeft) return BottomLeftCorner;
        if (onBottom && onRight) return BottomRightCorner;
        if (onTop) return TopEdge;
        if (onBottom) return BottomEdge;
        if (onLeft) return LeftEdge;
        if (onRight) return RightEdge;
    }
    
    return NoEdge;
}

void FloatingPanelBase::updateCursor(const QPoint &pos)
{
    if (m_resizing || !m_isResizable) return;
    
    ResizeEdge edge = getResizeEdge(pos);
    switch (edge) {
        case TopEdge:
        case BottomEdge:
            setCursor(Qt::SizeVerCursor);
            break;
        case LeftEdge:
        case RightEdge:
            setCursor(Qt::SizeHorCursor);
            break;
        case TopLeftCorner:
        case BottomRightCorner:
            setCursor(Qt::SizeFDiagCursor);
            break;
        case TopRightCorner:
        case BottomLeftCorner:
            setCursor(Qt::SizeBDiagCursor);
            break;
        default:
            // Only set arrow cursor if we're not over the title label
            if (!m_titleLabel || !m_titleLabel->geometry().contains(pos)) {
                setCursor(Qt::ArrowCursor);
            }
            break;
    }
}