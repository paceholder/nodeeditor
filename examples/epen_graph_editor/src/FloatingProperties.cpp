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

FloatingProperties::FloatingProperties(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Properties")
    , m_currentNodeId(-1)
    , _properties(nullptr)
{
    // Set panel-specific dimensions
    setFloatingWidth(200);
    setDockedWidth(250);  // Wider for properties

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

    // Additional style for property widgets
    QString additionalStyle = 
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
        "}";
    
    getContentWidget()->setStyleSheet(getContentWidget()->styleSheet() + additionalStyle);

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

    int i = 0;
    QtProperty *topItem = variantManager->addProperty(QtVariantPropertyManager::groupTypeId(),
                                                      QString::number(i++)
                                                          + QLatin1String(" Group Property"));

    QtVariantProperty *item = variantManager->addProperty(QVariant::Bool,
                                                          QString::number(i++)
                                                              + QLatin1String(" Bool Property"));
    item->setValue(true);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Int,
                                       QString::number(i++) + QLatin1String(" Int Property"));
    item->setValue(20);
    item->setAttribute(QLatin1String("minimum"), 0);
    item->setAttribute(QLatin1String("maximum"), 100);
    item->setAttribute(QLatin1String("singleStep"), 10);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Int,
                                       QString::number(i++)
                                           + QLatin1String(" Int Property (ReadOnly)"));
    item->setValue(20);
    item->setAttribute(QLatin1String("minimum"), 0);
    item->setAttribute(QLatin1String("maximum"), 100);
    item->setAttribute(QLatin1String("singleStep"), 10);
    item->setAttribute(QLatin1String("readOnly"), true);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Double,
                                       QString::number(i++) + QLatin1String(" Double Property"));
    item->setValue(1.2345);
    item->setAttribute(QLatin1String("singleStep"), 0.1);
    item->setAttribute(QLatin1String("decimals"), 3);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::String,
                                       QString::number(i++) + QLatin1String(" String Property"));
    item->setValue("Value");
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Date,
                                       QString::number(i++) + QLatin1String(" Date Property"));
    item->setValue(QDate::currentDate().addDays(2));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Time,
                                       QString::number(i++) + QLatin1String(" Time Property"));
    item->setValue(QTime::currentTime());
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::DateTime,
                                       QString::number(i++) + QLatin1String(" DateTime Property"));
    item->setValue(QDateTime::currentDateTime());
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Point,
                                       QString::number(i++) + QLatin1String(" Point Property"));
    item->setValue(QPoint(10, 10));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Size,
                                       QString::number(i++) + QLatin1String(" Size Property"));
    item->setValue(QSize(20, 20));
    item->setAttribute(QLatin1String("minimum"), QSize(10, 10));
    item->setAttribute(QLatin1String("maximum"), QSize(30, 30));
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QtVariantPropertyManager::enumTypeId(),
                                       QString::number(i++) + QLatin1String(" Enum Property"));
    QStringList enumNames;
    enumNames << "Enum0" << "Enum1" << "Enum2";
    item->setAttribute(QLatin1String("enumNames"), enumNames);
    item->setValue(1);
    topItem->addSubProperty(item);

    item = variantManager->addProperty(QVariant::Color,
                                       QString::number(i++) + QLatin1String(" Color Property"));
    topItem->addSubProperty(item);

    QtVariantEditorFactory *variantFactory = new QtVariantEditorFactory();

    QtTreePropertyBrowser *variantEditor = new QtTreePropertyBrowser();
    variantEditor->setFactoryForManager(variantManager, variantFactory);
    variantEditor->addProperty(topItem);
    variantEditor->setPropertiesWithoutValueMarked(true);
    variantEditor->setRootIsDecorated(false);

    return variantEditor;
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
    typeCombo->setCurrentIndex(nodeId % 5); // Example selection
    connect(typeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
        emit propertyChanged("type", index);
    });
    addPropertyWidget("Type:", typeCombo);

    // Position properties
    QDoubleSpinBox *xSpin = new QDoubleSpinBox();
    xSpin->setRange(-9999, 9999);
    xSpin->setValue(100.0 * nodeId); // Example value
    xSpin->setSuffix(" px");
    connect(xSpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        emit propertyChanged("x", value);
    });
    addPropertyWidget("X Position:", xSpin);

    QDoubleSpinBox *ySpin = new QDoubleSpinBox();
    ySpin->setRange(-9999, 9999);
    ySpin->setValue(50.0 * nodeId); // Example value
    ySpin->setSuffix(" px");
    connect(ySpin, QOverload<double>::of(&QDoubleSpinBox::valueChanged), [this](double value) {
        emit propertyChanged("y", value);
    });
    addPropertyWidget("Y Position:", ySpin);

    // Add some spacing at the end
    m_propertiesLayout->addSpacing(10);

    // Update the content size
    getContentWidget()->adjustSize();
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

void FloatingProperties::resizeEvent(QResizeEvent *event)
{
    FloatingPanelBase::resizeEvent(event);
    
    if (_properties) {
        _properties->setFixedHeight(height());
    }
}