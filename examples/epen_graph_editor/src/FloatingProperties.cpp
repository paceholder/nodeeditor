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

FloatingProperties::FloatingProperties(GraphEditorWindow *parent)
    : FloatingPanelBase(parent, "Properties")
    , m_currentNodeId(InvalidNodeId)
    , _currentNode(nullptr)
    , _properties(nullptr)
{
    setFloatingWidth(200);
    setDockedWidth(250);

    setupUI();
    connectSignals();

    setFixedWidth(floatingWidth());
    if (parent) {
        m_floatingGeometry = QRect(parent->width() - 220, 20, floatingWidth(), height());
        setGeometry(m_floatingGeometry);
    }

    raise();
    m_floatHeight = height();

    setDockPosition(DockPosition::DockedRight);
}

FloatingProperties::~FloatingProperties()
{
    delete _properties;
}

void FloatingProperties::setupUI()
{
    setupBaseUI("Properties");

    QFont labelFont = QApplication::font();
    labelFont.setPointSize(10);

    QVBoxLayout *layout = getContentLayout();

    _properties = new QtTreePropertyBrowser();

    const int ROW_HEIGHT = 50;    // Total row height - CHANGE THIS VALUE
    const int EDITOR_HEIGHT = 10; // Input field height (should be less than row height)
    const int EDITOR_PADDING = 2; // Padding inside editors

    QString styleSheet = QString("QtTreePropertyBrowser {"
                                 "   background-color: #ffffff;"
                                 "   alternate-background-color: #f9f9f9;"
                                 "   font-size: 11px;"
                                 "   height: %1px;" // Row height
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
                             .arg(0)
                             .arg(EDITOR_HEIGHT)
                             .arg(EDITOR_PADDING)
                             .arg(EDITOR_HEIGHT / 2);

    _properties->setStyleSheet(styleSheet);

    layout->addWidget(_properties);

    layout->addStretch();

    // Initial size
    getContentWidget()->adjustSize();
    adjustSize();
}

void FloatingProperties::connectSignals() {}

void FloatingProperties::clearProperties()
{
    m_currentNodeId = InvalidNodeId;
    if (_currentNode != nullptr) {
        _currentNode->deselected();
        _currentNode = nullptr;
    }
    if (_properties) {
        _properties->clear();
    }
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

    node->setupProperties(_properties);
}