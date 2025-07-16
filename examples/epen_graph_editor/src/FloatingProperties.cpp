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
#include <QVBoxLayout>

// Custom editor factory to create larger input fields
class CustomVariantEditorFactory : public QtVariantEditorFactory
{
public:
    QWidget *createEditor(QtVariantPropertyManager *manager,
                          QtProperty *property,
                          QWidget *parent) override
    {
        QWidget *editor = QtVariantEditorFactory::createEditor(manager, property, parent);

        if (editor) {
            // Set minimum height for all editors
            editor->setMinimumHeight(20);
            editor->setMaximumHeight(20);

            // Apply consistent styling to all editor types
            QString editorStyle
                = "padding: 2px 4px; margin: 0px; min-height: 20px; max-height: 20px;";

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
};

FloatingProperties::FloatingProperties(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Properties")
    , m_currentNodeId(InvalidNodeId)
    , _properties(nullptr)
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
    delete _properties;
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
    QtVariantPropertyManager *variantManager = new QtVariantPropertyManager();

    // Use custom factory with larger inputs
    CustomVariantEditorFactory *variantFactory = new CustomVariantEditorFactory();

    QtTreePropertyBrowser *variantEditor = new QtTreePropertyBrowser();
    variantEditor->setFactoryForManager(variantManager, variantFactory);

    // Create a sample property
    QtVariantProperty *item = variantManager->addProperty(QVariant::String, QLatin1String("Name"));
    item->setValue("Node001");

    variantEditor->addProperty(item);
    variantEditor->setPropertiesWithoutValueMarked(false);
    variantEditor->setRootIsDecorated(false);

    // Apply comprehensive styling for larger input fields
    variantEditor->setStyleSheet("QtTreePropertyBrowser {"
                                 "   background-color: #ffffff;"
                                 "   alternate-background-color: #f9f9f9;"
                                 "   font-size: 11px;"
                                 "}"
                                 "QtTreePropertyBrowser::item {"
                                 "   height: 20px;" // Increased row height
                                 "   padding-top: 0px;"
                                 "   padding-bottom: 0px;"
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
                                 "   min-height: 14px;"
                                 "   max-height: 14px;"
                                 "   padding: 0px;"
                                 "   margin: 0px;"
                                 "   border: 1px solid #bbb;"
                                 "   border-radius: 2px;"
                                 "   background-color: white;"
                                 "   font-size: 13px;"
                                 "}"
                                 // Focus style
                                 "QtTreePropertyBrowser QLineEdit:focus,"
                                 "QtTreePropertyBrowser QSpinBox:focus,"
                                 "QtTreePropertyBrowser QDoubleSpinBox:focus,"
                                 "QtTreePropertyBrowser QComboBox:focus {"
                                 "   border-color: #0078d4;"
                                 "   padding: 0px;"
                                 "   margin: 0px;"
                                 "   background-color: #f0f8ff;"
                                 "}"
                                 // Style for spin box buttons
                                 "QtTreePropertyBrowser QSpinBox::up-button,"
                                 "QtTreePropertyBrowser QDoubleSpinBox::up-button {"
                                 "   width: 16px;"
                                 "   height: 13px;"
                                 "   border-left: 1px solid #bbb;"
                                 "}"
                                 "QtTreePropertyBrowser QSpinBox::down-button,"
                                 "QtTreePropertyBrowser QDoubleSpinBox::down-button {"
                                 "   width: 16px;"
                                 "   height: 13px;"
                                 "   border-left: 1px solid #bbb;"
                                 "}"
                                 // Color button specific style
                                 "QtTreePropertyBrowser QtColorEditWidget {"
                                 "   min-height: 13px;"
                                 "   max-height: 13px;"
                                 "}"
                                 // Checkbox style
                                 "QtTreePropertyBrowser QCheckBox {"
                                 "   padding: 4px;"
                                 "}"
                                 "QtTreePropertyBrowser QCheckBox::indicator {"
                                 "   width: 16px;"
                                 "   height: 16px;"
                                 "}");

    // Set alternating row colors
    variantEditor->setAlternatingRowColors(true);

    // Adjust splitter position for better property name visibility
    variantEditor->setSplitterPosition(100);

    // Set indentation
    variantEditor->setIndentation(15);

    // Make sure headers are visible if needed
    variantEditor->setHeaderVisible(false);

    return variantEditor;
}

void FloatingProperties::connectSignals()
{
    // Connect to property change signals when values are modified
}

void FloatingProperties::updatePropertiesForNode(NodeId nodeId)
{
    if (m_currentNodeId == nodeId) {
        return; // Already showing this node's properties
    }

    m_currentNodeId = nodeId;
    clearPropertyWidgets();
}

void FloatingProperties::clearProperties()
{
    m_currentNodeId = InvalidNodeId;
    clearPropertyWidgets();

    QLabel *noSelectionLabel = new QLabel("No node selected");
    noSelectionLabel->setAlignment(Qt::AlignCenter);
    noSelectionLabel->setStyleSheet("color: #999; font-style: italic; padding: 0px;");
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

void FloatingProperties::resizeEvent(QResizeEvent *event)
{
    FloatingPanelBase::resizeEvent(event);

    if (_properties) {
        _properties->setFixedHeight(height());
    }
}