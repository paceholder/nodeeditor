#include "panels/FloatingToolbar.hpp"
#include "GraphEditorMainWindow.hpp"
#include <QAction>
#include <QApplication>
#include <QDebug>
#include <QFrame>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>

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

DraggableButton *FloatingToolbar::createNodeButton(const NodeButtonInfo &info, QWidget *parent)
{
    DraggableButton *btn = new DraggableButton(info.actionName, parent);
    QString buttonText = createSafeButtonText(info.icon, info.name);
    btn->setText(buttonText);
    btn->setToolTip(info.tooltip);
    btn->setEnabled(info.enabled);
    btn->setFont(m_buttonFont);
    btn->setProperty("nodeType", info.name);

    // Style for node buttons
    btn->setStyleSheet("DraggableButton {"
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
                       "}");

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

    QString viewButtonStyle = "QPushButton {"
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

    // Other Nodes section (always visible)
    QLabel *otherNodesLabel = new QLabel("Other Nodes:");
    otherNodesLabel->setStyleSheet("font-weight: bold; color: #333; padding: 5px;");
    layout->addWidget(otherNodesLabel);

    // Container for other nodes buttons
    QWidget *otherNodesContainer = new QWidget();
    QVBoxLayout *otherNodesLayout = new QVBoxLayout(otherNodesContainer);
    otherNodesLayout->setContentsMargins(20, 0, 5, 5);
    otherNodesLayout->setSpacing(2);

    // Create Other Nodes buttons
    QVector<NodeButtonInfo> otherNodeButtons
        = {{"Video Input", "◀", "<", "Create a video input node", "VideoInput", false},
           {"Video Output", "▶", ">", "Create a video output node", "VideoOutput", true},
           {"Process", "♦", "*", "Create a processing node", "Process", true},
           {"Callback Managed Image",
            "⬛",
            "=",
            "Create a Callback Managed Image node",
            "CallbackManagedImage",
            true}};

    for (const auto &buttonInfo : otherNodeButtons) {
        DraggableButton *btn = createNodeButton(buttonInfo, otherNodesContainer);
        otherNodesLayout->addWidget(btn);
        m_otherNodeButtons.append(btn);
    }

    layout->addWidget(otherNodesContainer);

    // Add separator
    addSeparator(layout);

    // Node Selection Label
    QLabel *nodeLabel = new QLabel("Node Selection:");
    nodeLabel->setStyleSheet("font-weight: bold; color: #333; padding: 5px;");
    layout->addWidget(nodeLabel);

    // Category combo box
    m_categoryCombo = new QComboBox();

    layout->addWidget(m_categoryCombo);

    // Subcategory combo box
    m_subCategoryCombo = new QComboBox();
    m_subCategoryCombo->hide(); // Initially hidden
    layout->addWidget(m_subCategoryCombo);

    // Container for node buttons
    m_nodeButtonContainer = new QWidget();
    m_nodeButtonLayout = new QVBoxLayout(m_nodeButtonContainer);
    m_nodeButtonLayout->setContentsMargins(5, 5, 5, 5);
    m_nodeButtonLayout->setSpacing(2);
    layout->addWidget(m_nodeButtonContainer);

    // Initialize categories data
    // Scalar category with subcategories
    Category scalarCategory;
    scalarCategory.name = "Scalar";

    // unsigned int subcategory
    SubCategory unsignedIntSub;
    unsignedIntSub.name = "unsigned int";
    unsignedIntSub.buttons = {{"Slider Input Buffer",
                               QString::fromUtf8("\u2B30"),
                               ">>",
                               "Create a UI unsigned int Slider Buffer node",
                               "Scalar_UnsignedInt_Slider",
                               true},
                              {"Plain Number Input Buffer",
                               QString::fromUtf8("\u2B30"),
                               ">>",
                               "Create a UI unsigned int Plain Number Buffer node",
                               "Scalar_UnsignedInt_Plain",
                               true},
                              {"Fixed Buffer",
                               QString::fromUtf8("\u2B30"),
                               ">>",
                               "Create an unsigned int fixed Buffer node",
                               "Scalar_UnsignedInt_Fixed",
                               true}};
    scalarCategory.subCategories.append(unsignedIntSub);

    // int subcategory
    SubCategory intSub;
    intSub.name = "int";
    intSub.buttons = {{"Slider Input Buffer",
                       QString::fromUtf8("\u2B30"),
                       ">>",
                       "Create a UI int Slider Buffer node",
                       "Scalar_Int_Slider",
                       true},
                      {"Plain Number Input Buffer",
                       QString::fromUtf8("\u2B30"),
                       ">>",
                       "Create a UI int Plain Number Buffer node",
                       "Scalar_Int_Plain",
                       true},
                      {"Fixed Buffer",
                       QString::fromUtf8("\u2B30"),
                       ">>",
                       "Create an int fixed Buffer node",
                       "Scalar_Int_Fixed",
                       true}};
    scalarCategory.subCategories.append(intSub);

    // double subcategory
    SubCategory doubleSub;
    doubleSub.name = "double";
    doubleSub.buttons = {{"Slider Input Buffer",
                          QString::fromUtf8("\u2B30"),
                          ">>",
                          "Create a UI double Slider Buffer node",
                          "Scalar_Double_Slider",
                          true},
                         {"Plain Number Input Buffer",
                          QString::fromUtf8("\u2B30"),
                          ">>",
                          "Create a UI double Plain Number Buffer node",
                          "Scalar_Double_Plain",
                          true},
                         {"Fixed Buffer",
                          QString::fromUtf8("\u2B30"),
                          ">>",
                          "Create a double fixed Buffer node",
                          "Scalar_Double_Fixed",
                          true}};
    scalarCategory.subCategories.append(doubleSub);

    // float subcategory
    SubCategory floatSub;
    floatSub.name = "float";
    floatSub.buttons = {{"Slider Input Buffer",
                         QString::fromUtf8("\u2B30"),
                         ">>",
                         "Create a UI float Slider Buffer node",
                         "Scalar_Float_Slider",
                         true},
                        {"Plain Number Input Buffer",
                         QString::fromUtf8("\u2B30"),
                         ">>",
                         "Create a UI float Plain Number Buffer node",
                         "Scalar_Float_Plain",
                         true},
                        {"Fixed Buffer",
                         QString::fromUtf8("\u2B30"),
                         ">>",
                         "Create a float Fixed Buffer node",
                         "Scalar_Float_Fixed",
                         true}};
    scalarCategory.subCategories.append(floatSub);

    // bool subcategory
    SubCategory boolSub;
    boolSub.name = "bool";
    boolSub.buttons = {{"Checkbox Input Buffer",
                        QString::fromUtf8("\u2B30"),
                        ">>",
                        "Create a UI boolean Checkbox Buffer node",
                        "Scalar_Boolean_Checkbox",
                        true},
                       {"Fixed Buffer",
                        QString::fromUtf8("\u2B30"),
                        ">>",
                        "Create a float Fixed Buffer node",
                        "Scalar_Boolean_Fixed",
                        true}};
    scalarCategory.subCategories.append(boolSub);

    // float4 subcategory
    SubCategory float4Sub;
    float4Sub.name = "float4";
    float4Sub.buttons = {{"Color Input Buffer",
                          QString::fromUtf8("\u2B30"),
                          ">>",
                          "Create a UI float4 Color Buffer node",
                          "Scalar_Float4_Color",
                          true},
                         {"Fixed Buffer",
                          QString::fromUtf8("\u2B30"),
                          ">>",
                          "Create a float4 Fixed Buffer node",
                          "Scalar_Float4_Fixed",
                          true}};
    scalarCategory.subCategories.append(float4Sub);

    m_categories.append(scalarCategory);

    // Array category with subcategories
    Category arrayCategory;
    arrayCategory.name = "Array";
    
    // Double array subcategory
    SubCategory arrayDoubleSub;
    arrayDoubleSub.name = "Double";
    arrayDoubleSub.buttons = {{"Fixed Double Buffer",
                               QString::fromUtf8("\u2B30"),
                               ">>",
                               "Create a double Fixed Array Buffer node",
                               "Array_Double_Fixed",
                               true},
                              {"Input Double Array Callback Managed Buffer",
                               QString::fromUtf8("\u2B30"),
                               ">>",
                               "Create a double Input Callback Managed Array Buffer node",
                               "Array_Double_InputCallback",
                               true},
                              {"Output Double Array Callback Managed Buffer",
                               QString::fromUtf8("\u2B30"),
                               ">>",
                               "Create a double Output Callback Managed Array Buffer node",
                               "Array_Double_OutputCallback",
                               true}};
    arrayCategory.subCategories.append(arrayDoubleSub);
    
    // Float array subcategory
    SubCategory arrayFloatSub;
    arrayFloatSub.name = "float";
    arrayFloatSub.buttons = {{"Fixed Float Buffer",
                              QString::fromUtf8("\u2B30"),
                              ">>",
                              "Create a float Fixed Array Buffer node",
                              "Array_Float_Fixed",
                              true},
                             {"Input Float Array Callback Managed Buffer",
                              QString::fromUtf8("\u2B30"),
                              ">>",
                              "Create a float Input Callback Managed Array Buffer node",
                              "Array_Float_InputCallback",
                              true},
                             {"Output Float Array Callback Managed Buffer",
                              QString::fromUtf8("\u2B30"),
                              ">>",
                              "Create a float Output Callback Managed Array Buffer node",
                              "Array_Float_OutputCallback",
                              true}};
    arrayCategory.subCategories.append(arrayFloatSub);
    
    // Int array subcategory
    SubCategory arrayIntSub;
    arrayIntSub.name = "int";
    arrayIntSub.buttons = {{"Fixed Int Buffer",
                            QString::fromUtf8("\u2B30"),
                            ">>",
                            "Create an int Fixed Array Buffer node",
                            "Array_Int_Fixed",
                            true},
                           {"Input Int Array Callback Managed Buffer",
                            QString::fromUtf8("\u2B30"),
                            ">>",
                            "Create an int Input Callback Managed Array Buffer node",
                            "Array_Int_InputCallback",
                            true},
                           {"Output Int Array Callback Managed Buffer",
                            QString::fromUtf8("\u2B30"),
                            ">>",
                            "Create an int Output Callback Managed Array Buffer node",
                            "Array_Int_OutputCallback",
                            true}};
    arrayCategory.subCategories.append(arrayIntSub);
    
    // Unsigned int array subcategory
    SubCategory arrayUnsignedIntSub;
    arrayUnsignedIntSub.name = "unsigned int";
    arrayUnsignedIntSub.buttons = {{"Fixed Unsigned Int Buffer",
                                    QString::fromUtf8("\u2B30"),
                                    ">>",
                                    "Create an unsigned int Fixed Array Buffer node",
                                    "Array_UnsignedInt_Fixed",
                                    true},
                                   {"Input Unsigned Int Array Callback Managed Buffer",
                                    QString::fromUtf8("\u2B30"),
                                    ">>",
                                    "Create an unsigned int Input Callback Managed Array Buffer node",
                                    "Array_UnsignedInt_InputCallback",
                                    true},
                                   {"Output Unsigned Int Array Callback Managed Buffer",
                                    QString::fromUtf8("\u2B30"),
                                    ">>",
                                    "Create an unsigned int Output Callback Managed Array Buffer node",
                                    "Array_UnsignedInt_OutputCallback",
                                    true}};
    arrayCategory.subCategories.append(arrayUnsignedIntSub);
    
    m_categories.append(arrayCategory);

    // Populate category combo box
    m_categoryCombo->addItem("Select Type...");
    for (const auto &category : m_categories) {
        m_categoryCombo->addItem(category.name);
    }

    // Connect signals
    connect(m_categoryCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &FloatingToolbar::onCategoryChanged);
    connect(m_subCategoryCombo,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &FloatingToolbar::onSubCategoryChanged);
}

void FloatingToolbar::onCategoryChanged(int index)
{
    clearNodeButtons();
    m_subCategoryCombo->clear();
    m_subCategoryCombo->hide();

    if (index <= 0 || index > m_categories.size()) {
        return;
    }

    const Category &category = m_categories[index - 1];

    // Check if this category has real subcategories (not just a single empty one)
    bool hasSubCategories = category.subCategories.size() > 1
                            || (category.subCategories.size() == 1
                                && !category.subCategories[0].name.isEmpty());

    if (hasSubCategories) {
        // Show subcategory combo
        m_subCategoryCombo->show();
        m_subCategoryCombo->addItem("Select Data Type...");
        for (const auto &subCategory : category.subCategories) {
            m_subCategoryCombo->addItem(subCategory.name);
        }
    } else {
        // No subcategories, show buttons directly
        if (!category.subCategories.isEmpty()) {
            updateNodeButtons();
        }
    }
}

void FloatingToolbar::onSubCategoryChanged(int index)
{
    clearNodeButtons();

    if (index <= 0) {
        return;
    }

    updateNodeButtons();
}

void FloatingToolbar::updateNodeButtons()
{
    int categoryIndex = m_categoryCombo->currentIndex();
    if (categoryIndex <= 0 || categoryIndex > m_categories.size()) {
        return;
    }

    const Category &category = m_categories[categoryIndex - 1];

    // Determine which subcategory to use
    int subCategoryIndex = 0;
    bool hasSubCategories = category.subCategories.size() > 1
                            || (category.subCategories.size() == 1
                                && !category.subCategories[0].name.isEmpty());

    if (hasSubCategories) {
        subCategoryIndex = m_subCategoryCombo->currentIndex() - 1;
        if (subCategoryIndex < 0 || subCategoryIndex >= category.subCategories.size()) {
            return;
        }
    }

    const SubCategory &subCategory = category.subCategories[subCategoryIndex];

    // Create buttons for the selected subcategory
    for (const auto &buttonInfo : subCategory.buttons) {
        DraggableButton *btn = createNodeButton(buttonInfo, m_nodeButtonContainer);
        m_nodeButtonLayout->addWidget(btn);
        m_currentNodeButtons.append(btn);
    }
}

void FloatingToolbar::clearNodeButtons()
{
    for (auto *btn : m_currentNodeButtons) {
        m_nodeButtonLayout->removeWidget(btn);
        btn->deleteLater();
    }
    m_currentNodeButtons.clear();
}

DraggableButton *FloatingToolbar::addNodeButton(
    QString name, QString icon, QString fallback, QString tooltip, bool enabled, QString actionName)
{
    DraggableButton *btn = new DraggableButton(actionName, this);
    QString buttonText = createSafeButtonText(icon, name);
    btn->setText(buttonText);
    btn->setToolTip(tooltip);
    btn->setEnabled(enabled);
    btn->setFont(m_buttonFont);
    btn->setProperty("nodeType", name);
    return btn;
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