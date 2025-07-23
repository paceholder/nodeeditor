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
    , _variantManager(new QtVariantPropertyManager())
    , _variantFactory(new QtVariantEditorFactory())
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

    _properties = new ObjectPropertyBrowser(this);

    _properties->setFactoryForManager(_variantManager, _variantFactory);
    _properties->setPropertiesWithoutValueMarked(true);
    _properties->setRootIsDecorated(false);

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
    unsetNode();
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
    if (_currentNode != nullptr) {
        _currentNode->deselected();
        _currentNode = nullptr;
    }
    _currentNode = node;
    _properties->setActiveObject(_currentNode);
}

void FloatingProperties::unsetNode()
{
    _currentNode = nullptr;
}