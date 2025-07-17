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
#include <QSpinBox>
#include <QStyledItemDelegate>
#include <QTimer>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QVBoxLayout>

// Custom item delegate for controlling row height
class PropertyBrowserDelegate : public QStyledItemDelegate
{
public:
    explicit PropertyBrowserDelegate(int rowHeight, QObject *parent = nullptr)
        : QStyledItemDelegate(parent)
        , m_rowHeight(rowHeight)
    {}

    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const override
    {
        QSize size = QStyledItemDelegate::sizeHint(option, index);
        size.setHeight(m_rowHeight);
        return size;
    }

    void setRowHeight(int height) { m_rowHeight = height; }

private:
    int m_rowHeight;
};

// Custom editor factory to create larger input fields
class CustomVariantEditorFactory : public QtVariantEditorFactory
{
public:
    CustomVariantEditorFactory(int editorHeight = 24)
        : m_editorHeight(editorHeight)
    {}

    QWidget *createEditor(QtVariantPropertyManager *manager,
                          QtProperty *property,
                          QWidget *parent) override
    {
        QWidget *editor = QtVariantEditorFactory::createEditor(manager, property, parent);

        if (editor) {
            // Set minimum height for all editors
            editor->setMinimumHeight(m_editorHeight);
            editor->setMaximumHeight(m_editorHeight);

            // Apply consistent styling to all editor types
            QString editorStyle
                = QString("padding: 2px 4px; margin: 0px; min-height: %1px; max-height: %1px;")
                      .arg(m_editorHeight);

            if (QLineEdit *lineEdit = qobject_cast<QLineEdit *>(editor)) {
                lineEdit->setStyleSheet("QLineEdit { " + editorStyle + " }");
            } else if (QSpinBox *spinBox = qobject_cast<QSpinBox *>(editor)) {
                spinBox->setStyleSheet("QSpinBox { " + editorStyle + " }");
            } else if (QDoubleSpinBox *doubleSpinBox = qobject_cast<QDoubleSpinBox *>(editor)) {
                doubleSpinBox->setStyleSheet("QDoubleSpinBox { " + editorStyle + " }");
            } else if (QComboBox *comboBox = qobject_cast<QComboBox *>(editor)) {
                comboBox->setStyleSheet("QComboBox { " + editorStyle + " }");
            }
        }

        return editor;
    }

    void setEditorHeight(int height) { m_editorHeight = height; }

private:
    int m_editorHeight;
};

FloatingProperties::FloatingProperties(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Properties")
    , m_currentNodeId(InvalidNodeId)
    , _properties(nullptr)
    , _variantManager(nullptr)
    , _variantFactory(nullptr)
{
    // Set panel-specific dimensions
    setFloatingWidth(200);
    setDockedWidth(250); // Wider for properties

    // Initialize UI
    setupUI();
    connectSignals();

    // Initial floating size and position
    setFixedWidth(floatingWidth());
    if (parent) {
        // Position on the right side by default
        m_floatingGeometry = QRect(parent->width() - 220, 20, floatingWidth(), height());
        setGeometry(m_floatingGeometry);
    }

    // Ensure properties panel is on top
    raise();
    m_floatHeight = height();

    // Start docked to the right by default
    setDockPosition(DockPosition::DockedRight);
}

FloatingProperties::~FloatingProperties()
{
    // The property browser will delete the manager and factory
    delete _properties;
    delete _variantManager;
    delete _variantFactory;
}

void FloatingProperties::setupUI()
{
    // Call base class setup
    setupBaseUI("Properties");

    QFont labelFont = QApplication::font();
    labelFont.setPointSize(10);

    QVBoxLayout *layout = getContentLayout();

    // Properties section
    m_propertiesLayout = new QVBoxLayout();
    m_propertiesLayout->setSpacing(6);
    layout->addLayout(m_propertiesLayout);

    // Initialize property browser
    _properties = getPropertyWidget();
    m_propertiesLayout->addWidget(_properties);

    layout->addStretch();

    // Initial size
    getContentWidget()->adjustSize();
    adjustSize();
}

QtTreePropertyBrowser *FloatingProperties::getPropertyWidget()
{
    _variantManager = new QtVariantPropertyManager();

    // Configuration for row and editor heights
    const int ROW_HEIGHT = 25;    // Total row height - CHANGE THIS VALUE
    const int EDITOR_HEIGHT = 18; // Input field height (should be less than row height)
    const int EDITOR_PADDING = 2; // Padding inside editors

    // Use custom factory with configurable editor height
    _variantFactory = new CustomVariantEditorFactory(EDITOR_HEIGHT);

    _variantEditor = new QtTreePropertyBrowser();
    _variantEditor->setFactoryForManager(_variantManager, _variantFactory);

    _variantEditor->setPropertiesWithoutValueMarked(false);
    _variantEditor->setRootIsDecorated(false);

    // Apply comprehensive styling with configurable heights
    QString styleSheet = QString("QtTreePropertyBrowser {"
                                 "   background-color: #ffffff;"
                                 "   alternate-background-color: #f9f9f9;"
                                 "   font-size: 11px;"
                                 "}"
                                 "QtTreePropertyBrowser::item {"
                                 "   height: %1px;" // Row height
                                 "   padding-top: %2px;"
                                 "   padding-bottom: %2px;"
                                 "}"
                                 // Style for property names column
                                 "QtTreePropertyBrowser::item:!has-children {"
                                 "   padding-left: 4px;"
                                 "}"
                                 // Global style for all input widgets
                                 "QtTreePropertyBrowser QLineEdit,"
                                 "QtTreePropertyBrowser QSpinBox,"
                                 "QtTreePropertyBrowser QDoubleSpinBox,"
                                 "QtTreePropertyBrowser QComboBox,"
                                 "QtTreePropertyBrowser QDateEdit,"
                                 "QtTreePropertyBrowser QTimeEdit,"
                                 "QtTreePropertyBrowser QDateTimeEdit {"
                                 "   min-height: %3px;"
                                 "   max-height: %3px;"
                                 "   padding: %4px 4px;"
                                 "   margin: 0px;"
                                 "   border: 1px solid #bbb;"
                                 "   border-radius: 2px;"
                                 "   background-color: white;"
                                 "   font-size: 11px;"
                                 "}"
                                 // Focus style
                                 "QtTreePropertyBrowser QLineEdit:focus,"
                                 "QtTreePropertyBrowser QSpinBox:focus,"
                                 "QtTreePropertyBrowser QDoubleSpinBox:focus,"
                                 "QtTreePropertyBrowser QComboBox:focus {"
                                 "   border-color: #0078d4;"
                                 "   background-color: #f0f8ff;"
                                 "}"
                                 // Style for spin box buttons
                                 "QtTreePropertyBrowser QSpinBox::up-button,"
                                 "QtTreePropertyBrowser QDoubleSpinBox::up-button {"
                                 "   width: 16px;"
                                 "   height: %5px;"
                                 "   border-left: 1px solid #bbb;"
                                 "}"
                                 "QtTreePropertyBrowser QSpinBox::down-button,"
                                 "QtTreePropertyBrowser QDoubleSpinBox::down-button {"
                                 "   width: 16px;"
                                 "   height: %5px;"
                                 "   border-left: 1px solid #bbb;"
                                 "}"
                                 // Color button specific style
                                 "QtTreePropertyBrowser QtColorEditWidget {"
                                 "   min-height: %3px;"
                                 "   max-height: %3px;"
                                 "}"
                                 // Checkbox style
                                 "QtTreePropertyBrowser QCheckBox {"
                                 "   padding: 2px;"
                                 "}"
                                 "QtTreePropertyBrowser QCheckBox::indicator {"
                                 "   width: 14px;"
                                 "   height: 14px;"
                                 "}")
                             .arg(ROW_HEIGHT)
                             .arg((ROW_HEIGHT - EDITOR_HEIGHT) / 2) // Vertical padding
                             .arg(EDITOR_HEIGHT)
                             .arg(EDITOR_PADDING)
                             .arg(EDITOR_HEIGHT / 2); // Spin button height

    _variantEditor->setStyleSheet(styleSheet);

    // IMPORTANT: Access the internal QTreeWidget to set row heights
    QTreeWidget *treeWidget = _variantEditor->findChild<QTreeWidget *>();
    if (treeWidget) {
        // Set uniform row heights
        treeWidget->setUniformRowHeights(true);

        // Apply custom delegate for row height
        PropertyBrowserDelegate *delegate = new PropertyBrowserDelegate(ROW_HEIGHT, treeWidget);
        treeWidget->setItemDelegate(delegate);

        // Additional tree widget styling
        treeWidget->setStyleSheet(treeWidget->styleSheet()
                                  + QString("QTreeWidget::item { height: %1px; }"
                                            "QTreeView::item { height: %1px; }")
                                        .arg(ROW_HEIGHT));

        // Force update after properties are added
        QTimer::singleShot(0, [treeWidget, ROW_HEIGHT]() {
            // Set size hint for all items
            QTreeWidgetItemIterator it(treeWidget);
            while (*it) {
                (*it)->setSizeHint(0, QSize(0, ROW_HEIGHT));
                ++it;
            }
        });
    }

    // Set alternating row colors
    _variantEditor->setAlternatingRowColors(true);

    // Adjust splitter position for better property name visibility
    _variantEditor->setSplitterPosition(100);

    // Set indentation
    _variantEditor->setIndentation(15);

    // Make sure headers are visible if needed
    _variantEditor->setHeaderVisible(false);

    return _variantEditor;
}

void FloatingProperties::connectSignals()
{
    // Connect to property change signals when values are modified
    if (_properties && _variantManager) {
        // Connect to value changed signal
        connect(_variantManager,
                &QtVariantPropertyManager::valueChanged,
                [this](QtProperty *property, const QVariant &value) {
                    qDebug() << "val chnaged" << value;
                    //emit propertyChanged(property->propertyName(), value);
                });
    }
}

void FloatingProperties::clearProperties()
{
    m_currentNodeId = InvalidNodeId;

    if (_properties) {
        _properties->clear();
    }
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

void FloatingProperties::resizeEvent(QResizeEvent *event)
{
    FloatingPanelBase::resizeEvent(event);

    if (_properties) {
        _properties->setFixedHeight(height());
    }
}

void FloatingProperties::setNode(OperationDataModel *node)
{
    _currentNode = node;
    _properties->clear();

    node->setupProperties(_variantEditor, _variantManager);
}