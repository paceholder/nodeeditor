#include "FloatingToolbar.hpp"
#include "GraphEditorMainWindow.hpp"
#include <QApplication>
#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMenu>
#include <QAction>

FloatingToolbar::FloatingToolbar(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Tools")
{
    // Set panel-specific dimensions
    setFloatingWidth(180);
    setDockedWidth(220);

    // Setup font
    m_buttonFont = QApplication::font();
#ifdef Q_OS_MAC
    m_buttonFont.setFamily("Helvetica Neue");
#elif defined(Q_OS_WIN)
    m_buttonFont.setFamily("Segoe UI");
#endif
    m_buttonFont.setPointSize(10);

    // Initialize UI
    setupUI();
    connectSignals();

    // Initial floating size and position
    setFixedWidth(floatingWidth());
    if (parent) {
        m_floatingGeometry = QRect((parent->width() - floatingWidth()) / 2,
                                   (parent->height() - height()) / 2,
                                   floatingWidth(),
                                   height());
        setGeometry(m_floatingGeometry);
    }

    // Ensure toolbar is on top
    raise();
    m_floatHeight = height();

    // Start docked to the left
    setDockPosition(DockPosition::DockedLeft);
}

QString FloatingToolbar::createSafeButtonText(const QString &icon, const QString &text)
{
    QFontMetrics fm(m_buttonFont);
    QRect boundingRect = fm.boundingRect(icon);

    if (boundingRect.width() < 5 || boundingRect.height() < 5) {
        // Return just the text without icon if icon is not properly supported
        return text;
    }

    return icon + " " + text;
}

DraggableButton* FloatingToolbar::createNodeButton(const NodeButtonInfo &info, QWidget *parent)
{
    DraggableButton *btn = new DraggableButton(info.actionName, parent);
    QString buttonText = createSafeButtonText(info.icon, info.name);
    btn->setText(buttonText);
    btn->setToolTip(info.tooltip);
    btn->setEnabled(info.enabled);
    btn->setFont(m_buttonFont);
    btn->setProperty("nodeType", info.name);
    
    // Style for node buttons
    btn->setStyleSheet(
        "DraggableButton {"
        "   padding: 4px 8px;"
        "   margin: 1px;"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "   background-color: white;"
        "   text-align: left;"
        "   font-size: 10px;"
        "}"
        "DraggableButton:hover {"
        "   background-color: #e8f0fe;"
        "   border-color: #4285f4;"
        "}"
        "DraggableButton:pressed {"
        "   background-color: #d2e3fc;"
        "}"
        "DraggableButton:disabled {"
        "   background-color: #f5f5f5;"
        "   color: #999;"
        "}"
    );
    
    return btn;
}

void FloatingToolbar::setupUI()
{
    // Call base class setup
    setupBaseUI("Tools");

    QVBoxLayout *layout = getContentLayout();
    layout->setSpacing(0);

    // Setup node categories
    setupNodeCategories();

    // Add separator before view controls
    addSeparator(layout);

    // View controls (not in expandable section)
    QLabel *viewLabel = new QLabel("View Controls:");
    viewLabel->setStyleSheet("font-weight: bold; color: #333; padding: 5px;");
    layout->addWidget(viewLabel);

    QWidget *viewControlsWidget = new QWidget();
    QVBoxLayout *viewLayout = new QVBoxLayout(viewControlsWidget);
    viewLayout->setContentsMargins(20, 0, 5, 5);
    viewLayout->setSpacing(2);

    QPushButton *btnZoomIn = new QPushButton(createSafeButtonText("+", "Zoom In"));
    QPushButton *btnZoomOut = new QPushButton(createSafeButtonText("-", "Zoom Out"));
    QPushButton *btnResetView = new QPushButton(createSafeButtonText("R", "Reset View"));

    btnZoomIn->setFont(m_buttonFont);
    btnZoomOut->setFont(m_buttonFont);
    btnResetView->setFont(m_buttonFont);

    QString viewButtonStyle = 
        "QPushButton {"
        "   padding: 4px 8px;"
        "   margin: 1px;"
        "   border: 1px solid #ccc;"
        "   border-radius: 3px;"
        "   background-color: white;"
        "   text-align: left;"
        "   font-size: 10px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #f0f0f0;"
        "   border-color: #999;"
        "}"
        "QPushButton:pressed {"
        "   background-color: #e0e0e0;"
        "}";

    btnZoomIn->setStyleSheet(viewButtonStyle);
    btnZoomOut->setStyleSheet(viewButtonStyle);
    btnResetView->setStyleSheet(viewButtonStyle);

    viewLayout->addWidget(btnZoomIn);
    viewLayout->addWidget(btnZoomOut);
    viewLayout->addWidget(btnResetView);

    layout->addWidget(viewControlsWidget);

    connect(btnZoomIn, &QPushButton::clicked, this, &FloatingToolbar::zoomInRequested);
    connect(btnZoomOut, &QPushButton::clicked, this, &FloatingToolbar::zoomOutRequested);
    connect(btnResetView, &QPushButton::clicked, this, &FloatingToolbar::resetViewRequested);

    layout->addStretch();

    // Initial size
    getContentWidget()->adjustSize();
    adjustSize();
}

void FloatingToolbar::setupNodeCategories()
{
    QVBoxLayout *layout = getContentLayout();
    
    // Define data types
    QVector<QString> dataTypes = {"unsigned int", "float", "double"};
    
    // Define node types for each data type
    QVector<NodeButtonInfo> nodeTypes = {
        {"Slider", "S", "S", "Slider input node", "SliderBuffer", true},
        {"Checkbox", "C", "C", "Checkbox input node", "CheckboxBuffer", true},
        {"Color4", "P", "P", "Color4 input node", "Color4Buffer", true}
    };
    
    // Create Scalar category
    ExpandableCategoryWidget *scalarCategory = new ExpandableCategoryWidget("Scalar", 0);
    m_categories["Scalar"] = scalarCategory;
    layout->addWidget(scalarCategory);
    
    // Add subcategories for Scalar
    for (const QString &dataType : dataTypes) {
        ExpandableCategoryWidget *typeCategory = new ExpandableCategoryWidget(dataType, 1);
        m_categories[QString("Scalar_%1").arg(dataType)] = typeCategory;
        
        // Create buttons for this type
        QVector<NodeButtonInfo> buttons;
        for (const auto &nodeType : nodeTypes) {
            NodeButtonInfo info = nodeType;
            info.name = QString("%1 %2").arg(dataType).arg(nodeType.name);
            info.actionName = QString("%1_%2_%3").arg("Scalar").arg(dataType).arg(nodeType.actionName);
            info.tooltip = QString("Create %1 %2 for Scalar").arg(dataType).arg(nodeType.tooltip);
            buttons.append(info);
        }
        
        addNodeButtonsToCategory(typeCategory, buttons);
        scalarCategory->addWidget(typeCategory);
    }
    
    // Create Array category
    ExpandableCategoryWidget *arrayCategory = new ExpandableCategoryWidget("Array", 0);
    m_categories["Array"] = arrayCategory;
    layout->addWidget(arrayCategory);
    
    // Add subcategories for Array
    for (const QString &dataType : dataTypes) {
        ExpandableCategoryWidget *typeCategory = new ExpandableCategoryWidget(dataType, 1);
        m_categories[QString("Array_%1").arg(dataType)] = typeCategory;
        
        // Create buttons for this type
        QVector<NodeButtonInfo> buttons;
        for (const auto &nodeType : nodeTypes) {
            NodeButtonInfo info = nodeType;
            info.name = QString("%1[] %2").arg(dataType).arg(nodeType.name);
            info.actionName = QString("%1_%2_%3").arg("Array").arg(dataType).arg(nodeType.actionName);
            info.tooltip = QString("Create %1 %2 for Array").arg(dataType).arg(nodeType.tooltip);
            buttons.append(info);
        }
        
        addNodeButtonsToCategory(typeCategory, buttons);
        arrayCategory->addWidget(typeCategory);
    }
    
    // Add separator
    addSeparator(layout);
    
    // Add Other Nodes category
    ExpandableCategoryWidget *otherCategory = new ExpandableCategoryWidget("Other Nodes", 0);
    m_categories["Other"] = otherCategory;
    layout->addWidget(otherCategory);
    
    QVector<NodeButtonInfo> otherNodes = {
        {"Video Input", "◀", "<", "Create a video input node", "VideoInput", true},
        {"Video Output", "▶", ">", "Create a video output node", "VideoOutput", false},
        {"Process", "♦", "*", "Create a processing node", "Process", true},
        {"Input Image", "⬆", "<<", "Create an Input Image node", "InImage", true},
        {"Output Image", "⬇", ">>", "Create an Output Image node", "OutImage", true},
        {"Fixed Buffer", "⬛", "=", "Create a Fixed Buffer node", "FixBuffer", true}
    };
    
    addNodeButtonsToCategory(otherCategory, otherNodes);
    
    // Set initial expanded state
    scalarCategory->setExpanded(false);
    arrayCategory->setExpanded(false);
    otherCategory->setExpanded(true);
}

void FloatingToolbar::addNodeButtonsToCategory(ExpandableCategoryWidget *category, 
                                              const QVector<NodeButtonInfo> &buttons)
{
    QWidget *container = new QWidget();
    QVBoxLayout *containerLayout = new QVBoxLayout(container);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->setSpacing(1);
    
    for (const auto &buttonInfo : buttons) {
        DraggableButton *btn = createNodeButton(buttonInfo, container);
        containerLayout->addWidget(btn);
    }
    
    category->setContentWidget(container);
}

void FloatingToolbar::addSeparator(QVBoxLayout *layout)
{
    layout->addSpacing(5);
    QFrame *separator = new QFrame();
    separator->setFrameShape(QFrame::HLine);
    separator->setFrameShadow(QFrame::Sunken);
    layout->addWidget(separator);
    layout->addSpacing(5);
}

void FloatingToolbar::connectSignals()
{
    if (getGraphEditor()) {
        connect(this, &FloatingToolbar::zoomInRequested, [this]() {
            getGraphEditor()->scale(1.2, 1.2);
        });

        connect(this, &FloatingToolbar::zoomOutRequested, [this]() {
            getGraphEditor()->scale(0.8, 0.8);
        });

        connect(this, &FloatingToolbar::resetViewRequested, [this]() {
            getGraphEditor()->resetTransform();
        });
    }
}