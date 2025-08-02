#include "ExpandableCategoryWidget.hpp"
#include <QApplication>
#include <QMouseEvent>
#include <QPainterPath>
#include <QTimer>

void ExpandableCategoryWidget::updateContentHeight()
{
    if (!m_isExpanded)
        return;

    // Calculate the new height needed
    m_contentArea->adjustSize();

    int totalHeight = 0;
    for (int i = 0; i < m_contentLayout->count(); ++i) {
        QWidget *widget = m_contentLayout->itemAt(i)->widget();
        if (widget) {
            // For expandable widgets, use their full size including content
            ExpandableCategoryWidget *expandable = qobject_cast<ExpandableCategoryWidget *>(widget);
            if (expandable) {
                // Get the total height: header + content area
                int widgetHeight = expandable->m_headerWidget->height();
                if (expandable->isExpanded() && expandable->m_contentArea) {
                    widgetHeight += expandable->m_contentArea->height();
                }
                totalHeight += widgetHeight;
            } else {
                totalHeight += widget->sizeHint().height();
            }
        }
    }

    // Add spacing and margins
    totalHeight += m_contentLayout->spacing() * qMax(0, m_contentLayout->count() - 1);
    totalHeight += m_contentLayout->contentsMargins().top()
                   + m_contentLayout->contentsMargins().bottom();

    int newHeight = qMax(totalHeight, 30);

    // Only animate if the height actually changed significantly (more than 1 pixel)
    if (qAbs(m_contentArea->height() - newHeight) > 1) {
        m_animation->stop();
        m_animation->setStartValue(m_contentArea->height());
        m_animation->setEndValue(newHeight);
        m_animation->start();
    }
}

bool ExpandableCategoryWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_headerWidget && event->type() == QEvent::MouseButtonRelease) {
        toggle();
        return true;
    }
    return QWidget::eventFilter(watched, event);
}

ExpandableCategoryWidget::ExpandableCategoryWidget(const QString &title,
                                                   int indentLevel,
                                                   QWidget *parent)
    : QWidget(parent)
    , m_isExpanded(false)
    , m_indentLevel(indentLevel)
    , m_title(title)
    , m_arrowColor(QColor(100, 100, 100))
{
    setupUI();

    // Setup animation
    m_animation = new QPropertyAnimation(this, "contentHeight");
    m_animation->setDuration(200);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);

    connect(m_toggleButton, &QPushButton::clicked, this, &ExpandableCategoryWidget::toggle);
    connect(m_animation, &QPropertyAnimation::valueChanged, this, [this]() {
        updateGeometry();
        update();
        // Notify parent to update while animating
        emit contentHeightChanged();
    });

    // Emit height changed when animation finishes
    connect(m_animation, &QPropertyAnimation::finished, this, [this]() {
        emit contentHeightChanged();
    });

    // Initially collapsed
    m_contentArea->setMaximumHeight(0);
    m_contentArea->setMinimumHeight(0);
}

void ExpandableCategoryWidget::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Header
    m_headerWidget = new QWidget(this);
    m_headerWidget->setMinimumHeight(28);
    m_headerWidget->setMaximumHeight(28);
    m_headerWidget->setCursor(Qt::PointingHandCursor);

    // Apply base style
    QString headerStyle = QString("QWidget {"
                                  "   background-color: %1;"
                                  "   border-bottom: 1px solid #ddd;"
                                  "}")
                              .arg(m_indentLevel == 0 ? "#f5f5f5" : "#fafafa");

    m_headerWidget->setStyleSheet(headerStyle);

    QHBoxLayout *headerLayout = new QHBoxLayout(m_headerWidget);
    headerLayout->setContentsMargins(m_indentLevel * 20 + 5, 0, 5, 0);
    headerLayout->setSpacing(5);

    // Toggle button with arrow (invisible, we'll draw the chevron)
    m_toggleButton = new QPushButton(m_headerWidget);
    m_toggleButton->setFixedSize(16, 16);
    m_toggleButton->setFlat(true);
    m_toggleButton->setCursor(Qt::PointingHandCursor);
    m_toggleButton->setStyleSheet("QPushButton {"
                                  "   border: none;"
                                  "   background-color: transparent;"
                                  "}");

    // Add chevron label
    QLabel *chevronLabel = new QLabel(m_headerWidget);
    chevronLabel->setFixedSize(16, 16);
    chevronLabel->setAlignment(Qt::AlignCenter);
    chevronLabel->setStyleSheet("QLabel { color: #666; font-size: 12px; }");
    chevronLabel->setText("▶"); // Right-pointing triangle
    m_chevronLabel = chevronLabel;

    // Title label
    m_titleLabel = new QLabel(m_title, m_headerWidget);
    m_titleLabel->setStyleSheet(QString("QLabel {"
                                        "   font-weight: %1;"
                                        "   color: #333;"
                                        "   font-size: %2px;"
                                        "}")
                                    .arg(m_indentLevel == 0 ? "bold" : "normal")
                                    .arg(m_indentLevel == 0 ? 12 : 11));

    headerLayout->addWidget(chevronLabel);
    headerLayout->addWidget(m_titleLabel);
    headerLayout->addStretch();

    // Content area
    m_contentArea = new QWidget(this);
    m_contentLayout = new QVBoxLayout(m_contentArea);
    m_contentLayout->setContentsMargins(0, 2, 0, 2);
    m_contentLayout->setSpacing(2);

    // Add to main layout
    mainLayout->addWidget(m_headerWidget);
    mainLayout->addWidget(m_contentArea);

    // Make header clickable
    m_headerWidget->installEventFilter(this);
}

void ExpandableCategoryWidget::setContentWidget(QWidget *widget)
{
    // Clear existing widgets
    QLayoutItem *item;
    while ((item = m_contentLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (widget) {
        m_contentLayout->addWidget(widget);
    }
}

QWidget *ExpandableCategoryWidget::contentWidget() const
{
    if (m_contentLayout->count() > 0) {
        return m_contentLayout->itemAt(0)->widget();
    }
    return nullptr;
}

void ExpandableCategoryWidget::addWidget(QWidget *widget)
{
    m_contentLayout->addWidget(widget);

    // Connect to subcategory height changes if it's an ExpandableCategoryWidget
    ExpandableCategoryWidget *expandableChild = qobject_cast<ExpandableCategoryWidget *>(widget);
    if (expandableChild) {
        connect(expandableChild,
                &ExpandableCategoryWidget::contentHeightChanged,
                this,
                &ExpandableCategoryWidget::updateContentHeight);
    }

    // If we're expanded, update the height to accommodate the new widget
    if (m_isExpanded) {
        QTimer::singleShot(0, [this]() { updateContentHeight(); });
    }
}

QVBoxLayout *ExpandableCategoryWidget::contentLayout() const
{
    return m_contentLayout;
}

void ExpandableCategoryWidget::setExpanded(bool expanded)
{
    if (m_isExpanded == expanded)
        return;

    m_isExpanded = expanded;

    // Update chevron
    if (m_chevronLabel) {
        m_chevronLabel->setText(expanded ? "▼" : "▶");
    }

    int targetHeight = 0;
    if (expanded) {
        // Calculate the actual height needed
        m_contentArea->setMaximumHeight(QWIDGETSIZE_MAX);
        m_contentArea->adjustSize();

        // Force layout update for all children
        QApplication::processEvents();

        // Calculate total height including nested expandable widgets
        int totalHeight = 0;
        for (int i = 0; i < m_contentLayout->count(); ++i) {
            QWidget *widget = m_contentLayout->itemAt(i)->widget();
            if (widget) {
                // For expandable widgets, we need their full height including expanded content
                ExpandableCategoryWidget *expandable = qobject_cast<ExpandableCategoryWidget *>(
                    widget);
                if (expandable) {
                    // Force the widget to calculate its size
                    expandable->adjustSize();
                    // Use the actual height which includes header + content (if expanded)
                    totalHeight += expandable->height();
                } else {
                    widget->adjustSize();
                    totalHeight += widget->sizeHint().height();
                }
            }
        }

        // Add spacing
        totalHeight += m_contentLayout->spacing() * qMax(0, m_contentLayout->count() - 1);
        totalHeight += m_contentLayout->contentsMargins().top()
                       + m_contentLayout->contentsMargins().bottom();

        targetHeight = qMax(totalHeight, 30); // Minimum height
    }

    m_animation->setStartValue(m_contentArea->height());
    m_animation->setEndValue(targetHeight);
    m_animation->start();

    emit expandedChanged(expanded);
    emit contentHeightChanged();
}

bool ExpandableCategoryWidget::isExpanded() const
{
    return m_isExpanded;
}

void ExpandableCategoryWidget::setIndentLevel(int level)
{
    m_indentLevel = level;
    // Update header indentation
    if (m_headerWidget && m_headerWidget->layout()) {
        m_headerWidget->layout()->setContentsMargins(m_indentLevel * 20 + 5, 0, 5, 0);
    }
}

int ExpandableCategoryWidget::indentLevel() const
{
    return m_indentLevel;
}

void ExpandableCategoryWidget::setHeaderStyle(const QString &style)
{
    m_headerWidget->setStyleSheet(style);
}

void ExpandableCategoryWidget::setArrowColor(const QColor &color)
{
    m_arrowColor = color;
    update();
}

void ExpandableCategoryWidget::toggle()
{
    setExpanded(!m_isExpanded);
}

void ExpandableCategoryWidget::expand()
{
    setExpanded(true);
}

void ExpandableCategoryWidget::collapse()
{
    setExpanded(false);
}

void ExpandableCategoryWidget::paintEvent(QPaintEvent *event)
{
    QWidget::paintEvent(event);
    // Removed custom arrow painting since we're using chevron characters
}

void ExpandableCategoryWidget::updateArrow()
{
    // Update chevron when state changes
    if (m_chevronLabel) {
        m_chevronLabel->setText(m_isExpanded ? "▼" : "▶");
    }
}

int ExpandableCategoryWidget::contentHeight() const
{
    return m_contentArea->height();
}

void ExpandableCategoryWidget::setContentHeight(int height)
{
    m_contentArea->setFixedHeight(height);
}
