#include "FloatingProperties.hpp"
#include "GraphEditorMainWindow.hpp"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDebug>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QPropertyAnimation>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>

FloatingProperties::FloatingProperties(GraphEditorWindow *parent)
    : QWidget(parent)
    , m_graphEditor(parent)
    , m_dragging(false)
    , m_dockPosition(Floating)
    , m_previewDockPosition(Floating)
    , m_dockMargin(0)
    , m_dockingDistance(40)
    , m_dockedWidth(250)  // Wider for properties
    , m_currentNodeId(-1)
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
    setFixedWidth(200);
    if (parent) {
        // Position on the right side by default
        m_floatingGeometry = QRect(parent->width() - 220,
                                   20,
                                   200,
                                   height());
        setGeometry(m_floatingGeometry);
    }

    // Ensure properties panel is on top
    raise();
    m_floatHeight = height();

    // Start docked to the right by default
    setDockPosition(DockPosition::DockedRight);
}

void FloatingProperties::setupUI()
{
    // Create scroll area for when docked
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    m_scrollArea->setFrameShape(QFrame::NoFrame);

    m_contentWidget = new QWidget();
    m_contentWidget->setObjectName("PropertiesContent");

    QFont labelFont = QApplication::font();
    labelFont.setPointSize(10);

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

    m_contentWidget->setStyleSheet("#PropertiesContent {"
                                   "   background-color: #f5f5f5;"
                                   "   border: 1px solid #ccc;"
                                   "   border-radius: 6px;"
                                   "}"
                                   "QLabel {"
                                   "   color: #555;"
                                   "   font-size: 10px;"
                                   "   font-weight: bold;"
                                   "   margin-top: 5px;"
                                   "}"
                                   "QLineEdit, QSpinBox, QDoubleSpinBox, QComboBox {"
                                   "   padding: 4px 6px;"
                                   "   border: 1px solid #bbb;"
                                   "   border-radius: 3px;"
                                   "   background-color: white;"
                                   "   font-size: 11px;"
                                   "}"
                                   "QLineEdit:focus, QSpinBox:focus, QDoubleSpinBox:focus, QComboBox:focus {"
                                   "   border-color: #0078d4;"
                                   "   outline: none;"
                                   "}"
                                   "QCheckBox {"
                                   "   font-size: 11px;"
                                   "   spacing: 5px;"
                                   "}"
                                   "QCheckBox::indicator {"
                                   "   width: 16px;"
                                   "   height: 16px;"
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
    QLabel *title = new QLabel("Properties");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-weight: bold;"
                         "padding: 8px;"
                         "background-color: #e0e0e0;"
                         "border-radius: 4px;"
                         "margin-bottom: 5px;");
    title->setCursor(Qt::SizeAllCursor);
    m_layout->addWidget(title);

    // Properties section
    m_propertiesLayout = new QVBoxLayout();
    m_propertiesLayout->setSpacing(6);
    m_layout->addLayout(m_propertiesLayout);

    // Initial "no selection" message
    QLabel *noSelectionLabel = new QLabel("No node selected");
    noSelectionLabel->setAlignment(Qt::AlignCenter);
    noSelectionLabel->setStyleSheet("color: #999; font-style: italic; padding: 20px;");
    m_propertiesLayout->addWidget(noSelectionLabel);

    m_layout->addStretch();

    // Initial size
    m_contentWidget->adjustSize();
    adjustSize();
}

void FloatingProperties::connectSignals()
{
    // Connect to property change signals when values are modified
}

void FloatingProperties::updatePropertiesForNode(int nodeId)
{
    if (m_currentNodeId == nodeId) {
        return; // Already showing this node's properties
    }

    m_currentNodeId = nodeId;
    clearPropertyWidgets();

    // Example properties - you would get these from your node model
    QLabel *nodeHeader = new QLabel(QString("Node #%1").arg(nodeId));
    nodeHeader->setStyleSheet("font-size: 12px; font-weight: bold; color: #333; padding: 5px 0;");
    m_propertiesLayout->addWidget(nodeHeader);
    m_propertyWidgets.append(nodeHeader);

    // Add separator
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    m_propertiesLayout->addWidget(separator);
    m_propertyWidgets.append(separator);

    // Example properties based on node type
    // You would customize this based on your actual node properties

    // Name property
    QLineEdit *nameEdit = new QLineEdit();
    nameEdit->setText(QString("Node_%1").arg(nodeId));
    connect(nameEdit, &QLineEdit::textChanged, [this](const QString &text) {
        emit propertyChanged("name", text);
    });
    addPropertyWidget("Name:", nameEdit);

    // Type property (read-only)
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItems({"Video Input", "Video Output", "Process", "Image", "Buffer"});
    typeCombo->setCurrentIndex(nodeId % 5);  // Example selection
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        emit propertyChanged("type", index);
    });
    addPropertyWidget("Type:", typeCombo);

    // Position properties
    QDoubleSpinBox *xSpin = new QDoubleSpinBox();
    xSpin->setRange(-9999, 9999);
    xSpin->setValue(100.0 * nodeId);  // Example value
    xSpin->setSuffix(" px");
    connect(xSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        emit propertyChanged("x", value);
    });
    addPropertyWidget("X Position:", xSpin);

    QDoubleSpinBox *ySpin = new QDoubleSpinBox();
    ySpin->setRange(-9999, 9999);
    ySpin->setValue(50.0 * nodeId);  // Example value
    ySpin->setSuffix(" px");
    connect(ySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        emit propertyChanged("y", value);
    });
    addPropertyWidget("Y Position:", ySpin);

    // Size properties
    QSpinBox *widthSpin = new QSpinBox();
    widthSpin->setRange(50, 500);
    widthSpin->setValue(150);
    widthSpin->setSuffix(" px");
    connect(widthSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        emit propertyChanged("width", value);
    });
    addPropertyWidget("Width:", widthSpin);

    QSpinBox *heightSpin = new QSpinBox();
    heightSpin->setRange(50, 500);
    heightSpin->setValue(100);
    heightSpin->setSuffix(" px");
    connect(heightSpin, QOverload<int>::of(&QSpinBox::valueChanged), [this](int value) {
        emit propertyChanged("height", value);
    });
    addPropertyWidget("Height:", heightSpin);

    // Enabled property
    QCheckBox *enabledCheck = new QCheckBox("Enabled");
    enabledCheck->setChecked(true);
    connect(enabledCheck, &QCheckBox::toggled, [this](bool checked) {
        emit propertyChanged("enabled", checked);
    });
    m_propertiesLayout->addWidget(enabledCheck);
    m_propertyWidgets.append(enabledCheck);

    // Add some spacing at the end
    m_propertiesLayout->addSpacing(10);
    
    // Update the content size
    m_contentWidget->adjustSize();
}

void FloatingProperties::clearProperties()
{
    m_currentNodeId = -1;
    clearPropertyWidgets();
    
    QLabel *noSelectionLabel = new QLabel("No node selected");
    noSelectionLabel->setAlignment(Qt::AlignCenter);
    noSelectionLabel->setStyleSheet("color: #999; font-style: italic; padding: 20px;");
    m_propertiesLayout->addWidget(noSelectionLabel);
    m_propertyWidgets.append(noSelectionLabel);
}

void FloatingProperties::clearPropertyWidgets()
{
    for (QWidget *widget : m_propertyWidgets) {
        m_propertiesLayout->removeWidget(widget);
        widget->deleteLater();
    }
    m_propertyWidgets.clear();
}

void FloatingProperties::addPropertyWidget(const QString &label, QWidget *widget)
{
    QLabel *labelWidget = new QLabel(label);
    m_propertiesLayout->addWidget(labelWidget);
    m_propertiesLayout->addWidget(widget);
    m_propertyWidgets.append(labelWidget);
    m_propertyWidgets.append(widget);
}

void FloatingProperties::updatePosition()
{
    if (m_dockPosition != Floating) {
        updateDockedGeometry();
    }
}

void FloatingProperties::setDockPosition(DockPosition position)
{
    if (m_dockPosition == position)
        return;

    m_dockPosition = position;

    if (position == Floating) {
        // Restore floating geometry
        setMinimumSize(0, 0);
        setMaximumSize(QWIDGETSIZE_MAX, QWIDGETSIZE_MAX);
        setFixedWidth(200);

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

FloatingProperties::DockPosition FloatingProperties::checkDockingZone(const QPoint &pos)
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

void FloatingProperties::applyDocking(DockPosition position)
{
    setDockPosition(position);
}

void FloatingProperties::updateDockedGeometry()
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

    m_contentWidget->setStyleSheet("#PropertiesContent {"
                                   "   background-color: #f5f5f5;"
                                   "   border: 1px solid #ccc;"
                                   "   border-radius: 0px;"
                                   "}");
}

void FloatingProperties::paintEvent(QPaintEvent *event)
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

void FloatingProperties::mousePressEvent(QMouseEvent *event)
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

void FloatingProperties::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && (event->buttons() & Qt::LeftButton)) {
        QPoint newPos = pos() + event->pos() - m_dragStartPosition;

        // If currently docked, undock first
        if (m_dockPosition != Floating) {
            m_dockPosition = Floating;
            setFixedWidth(200);
            // Adjust position to keep mouse on title bar
            newPos = QPoint(event->globalPosition().x() - m_dragStartPosition.x(),
                            event->globalPosition().y() - m_dragStartPosition.y());
            if (parentWidget()) {
                newPos = parentWidget()->mapFromGlobal(newPos);
            }
            setFixedHeight(m_floatHeight);
            m_contentWidget->setStyleSheet("#PropertiesContent {"
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

void FloatingProperties::mouseReleaseEvent(QMouseEvent *event)
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

void FloatingProperties::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    updatePosition();
    raise(); // Ensure properties panel is on top
}

void FloatingProperties::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    // If docked, maintain the docked state
    if (m_dockPosition != Floating) {
        updateDockedGeometry();
    }
}