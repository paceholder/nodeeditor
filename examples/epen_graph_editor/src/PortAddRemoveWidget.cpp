#include "PortAddRemoveWidget.hpp"

#include "DataFlowModel.hpp"

// Layout constants
static const int BUTTON_HEIGHT = 25;

//static const int FIRST_NODE_SPACING = 27;
static const int NODE_SPACING = 13;
static const int CENTER_SPACING = 50;
static const QString BUTTON_STYLE = "background-color: grey;padding: 5px;margin:0px;";
static const QString RADIO_STYLE = "QRadioButton { background-color: transparent; padding: 0px; "
                                   "margin: 0px; spacing: 0px; height:10px; }"
                                   "QRadioButton::indicator { width: 16px; height: 16px; }";

PortAddRemoveWidget::PortAddRemoveWidget(NodeId nodeId, DataFlowModel &model, QWidget *parent)
    : QWidget(parent)
    , _nodeId(nodeId)
    , _model(model)
{
    setStyleSheet("background-color: transparent;padding: 0px;");
    // Widget constructor with proper top-alignment
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

    // Create radio button group
    _radioGroup = new QButtonGroup(this);
    _radioGroup->setExclusive(true);

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setContentsMargins(0, 0, 0, 10);
    hl->setSpacing(0);

    // Create left layout WITHOUT stretch at the beginning
    _left = new QVBoxLayout();
    _left->setSpacing(NODE_SPACING);
    _left->setContentsMargins(0, 0, 0, 0);
    // Don't add stretch here - we'll add it at the end

    // Create right layout WITHOUT stretch at the beginning
    _right = new QVBoxLayout();
    _right->setSpacing(NODE_SPACING);
    _right->setContentsMargins(0, 0, 0, 0);
    // Don't add stretch here - we'll add it at the end

    hl->addLayout(_left);
    hl->addSpacing(CENTER_SPACING);
    hl->addLayout(_right);

    // LEFT SIDE: Add I and B buttons
    auto topButtonsLeftRow = new QHBoxLayout();
    topButtonsLeftRow->setContentsMargins(0, 0, 0, 0);
    topButtonsLeftRow->setSpacing(4); // Add small spacing between buttons

    _letMinusButton = new QPushButton("-");
    _letMinusButton->setFixedHeight(BUTTON_HEIGHT);
    topButtonsLeftRow->addWidget(_letMinusButton);
    _letMinusButton->setStyleSheet(BUTTON_STYLE);
    _letMinusButton->setEnabled(false);

    auto buttonILeft = new QPushButton("I");
    buttonILeft->setFixedHeight(BUTTON_HEIGHT);
    topButtonsLeftRow->addWidget(buttonILeft);
    buttonILeft->setStyleSheet(BUTTON_STYLE);

    auto buttonBLeft = new QPushButton("B");
    buttonBLeft->setFixedHeight(BUTTON_HEIGHT);
    topButtonsLeftRow->addWidget(buttonBLeft);
    buttonBLeft->setStyleSheet(BUTTON_STYLE);

    // Add the button layout to the top of left side
    _left->addLayout(topButtonsLeftRow);

    // Add 13-pixel spacer after buttons

    // Connect left buttons
    connect(buttonBLeft, &QPushButton::clicked, this, &PortAddRemoveWidget::addLeftPortB);
    connect(buttonILeft, &QPushButton::clicked, this, &PortAddRemoveWidget::addLeftPortI);
    connect(_letMinusButton, &QPushButton::clicked, this, &PortAddRemoveWidget::leftMinusClicked);

    // RIGHT SIDE: Add I and B buttons
    auto topButtonsRightRow = new QHBoxLayout();
    topButtonsRightRow->setContentsMargins(0, 0, 0, 0);
    topButtonsRightRow->setSpacing(4); // Add small spacing between buttons

    auto buttonI = new QPushButton("I");
    buttonI->setFixedHeight(BUTTON_HEIGHT);
    topButtonsRightRow->addWidget(buttonI);
    buttonI->setStyleSheet(BUTTON_STYLE);

    auto buttonB = new QPushButton("B");
    buttonB->setFixedHeight(BUTTON_HEIGHT);
    topButtonsRightRow->addWidget(buttonB);
    buttonB->setStyleSheet(BUTTON_STYLE);

    _rightMinusButton = new QPushButton("-");
    _rightMinusButton->setFixedHeight(BUTTON_HEIGHT);
    topButtonsRightRow->addWidget(_rightMinusButton);
    _rightMinusButton->setStyleSheet(BUTTON_STYLE);
    _rightMinusButton->setEnabled(false);

    // Add the button layout to the top of right side
    _right->addLayout(topButtonsRightRow);

    // Connect right buttons
    connect(buttonI, &QPushButton::clicked, this, &PortAddRemoveWidget::addRightPortI);
    connect(buttonB, &QPushButton::clicked, this, &PortAddRemoveWidget::addRightPortB);
    connect(_rightMinusButton, &QPushButton::clicked, this, &PortAddRemoveWidget::rightMinusClicked);

    // Add stretch at the END to push everything to the top
    _left->addStretch();
    _right->addStretch();
}

void PortAddRemoveWidget::addLeftPort(bool isImage)
{
    // Create radio button
    auto radioButton = new QRadioButton();
    radioButton->setFixedSize(20, BUTTON_HEIGHT);
    radioButton->setStyleSheet(RADIO_STYLE);
    _radioGroup->addButton(radioButton);
    connect(radioButton,
            &QRadioButton::toggled,
            this,
            &PortAddRemoveWidget::onLeftRadioButtonToggled);

    // Insert after + button (0), first port spacer (1), and existing ports
    _left->insertWidget(_leftPorts + 1, radioButton);

    // Trigger changes in the model
    _model.addProcessNodePort(_nodeId, PortType::In, _leftPorts, isImage);
    _leftPorts++;

    adjustSize();
}

void PortAddRemoveWidget::removeLeftPort() {}

void PortAddRemoveWidget::addRightPortI()
{
    addRightPort(true);
}

void PortAddRemoveWidget::addRightPortB()
{
    addRightPort(false);
}

void PortAddRemoveWidget::addLeftPortI()
{
    addLeftPort(true);
}

void PortAddRemoveWidget::addLeftPortB()
{
    addLeftPort(false);
}

void PortAddRemoveWidget::addRightPort(bool isImage)
{
    // Create radio button
    auto radioButton = new QRadioButton();
    radioButton->setFixedSize(20, BUTTON_HEIGHT);
    _radioGroup->addButton(radioButton);
    connect(radioButton,
            &QRadioButton::toggled,
            this,
            &PortAddRemoveWidget::onRightRadioButtonToggled);

    // Insert after + button (0), first port spacer (1), and existing ports
    _right->insertWidget(_rightPorts + 1, radioButton, 0, Qt::AlignRight);

    // Trigger changes in the model
    _model.addProcessNodePort(_nodeId, PortType::Out, _rightPorts, isImage);
    _rightPorts++;

    adjustSize();
}

void PortAddRemoveWidget::removeRightPort()
{
    // Find which button was clicked
    QPushButton *clickedButton = qobject_cast<QPushButton *>(QObject::sender());
    if (!clickedButton)
        return;

    // Find the widget containing the clicked button
    int widgetIndex = -1;
    QWidget *portWidget = nullptr;

    for (int i = 2; i < _right->count(); ++i) { // Start from 2 to skip buttons and spacer
        QLayoutItem *item = _right->itemAt(i);
        if (item && item->widget()) {
            QWidget *widget = item->widget();
            // Check if this widget contains our button
            QHBoxLayout *hLayout = qobject_cast<QHBoxLayout *>(widget->layout());
            if (hLayout) {
                for (int j = 0; j < hLayout->count(); ++j) {
                    if (hLayout->itemAt(j)->widget() == clickedButton) {
                        widgetIndex = i;
                        portWidget = widget;
                        break;
                    }
                }
            }
        }
        if (widgetIndex != -1)
            break;
    }

    if (widgetIndex == -1 || !portWidget)
        return;

    // Remove radio button from group
    QHBoxLayout *portLayout = qobject_cast<QHBoxLayout *>(portWidget->layout());
    if (portLayout) {
        for (int i = 0; i < portLayout->count(); ++i) {
            QRadioButton *radio = qobject_cast<QRadioButton *>(portLayout->itemAt(i)->widget());
            if (radio) {
                _radioGroup->removeButton(radio);
                break;
            }
        }
    }

    // Remove the widget
    auto item = _right->takeAt(widgetIndex);
    if (item) {
        item->widget()->deleteLater();
        delete item;
    }

    _rightPorts--;
    _rightMinusButton->setEnabled(false);

    // Trigger changes in the model
    _model.removeProcessNodePort(_nodeId, PortType::Out, widgetIndex - 1);

    adjustSize();
}

void PortAddRemoveWidget::onLeftRadioButtonToggled(bool checked)
{
    if (checked) {
        int radioIndex = findWhichRadioWasClicked(_left, sender());
        _letMinusButton->setEnabled(true);
        _rightMinusButton->setEnabled(false);
        _selectedLeftPortIndex = radioIndex;
        _selectedRightPortIndex = -1;
    }
}

void PortAddRemoveWidget::onRightRadioButtonToggled(bool checked)
{
    if (checked) {
        int radioIndex = findWhichRadioWasClicked(_right, sender());
        _rightMinusButton->setEnabled(true);
        _letMinusButton->setEnabled(false);
        _selectedRightPortIndex = radioIndex;
        _selectedLeftPortIndex = -1;
    }
}

PortAddRemoveWidget::~PortAddRemoveWidget()
{
    //
}

int PortAddRemoveWidget::findWhichRadioWasClicked(QVBoxLayout *layout, QObject *sender)
{
    for (int i = 0; i < layout->count(); ++i) {
        auto layoutItem = layout->itemAt(i);
        auto radioButton = dynamic_cast<QRadioButton *>(layoutItem->widget());

        if (!radioButton)
            continue;
        if (sender == radioButton) {
            return i - 1;
            break;
        }
    }
    return -1;
}

void PortAddRemoveWidget::rootSelected()
{
    _rightMinusButton->setEnabled(false);
    _letMinusButton->setEnabled(false);
    _selectedRightPortIndex = -1;
    _selectedLeftPortIndex = -1;
    _radioGroup->setExclusive(false); // Allow unchecking all
    for (QAbstractButton *btn : _radioGroup->buttons()) {
        btn->setChecked(false);
    }
    _radioGroup->setExclusive(true); // Restore exclusive behavior
}

void PortAddRemoveWidget::leftMinusClicked()
{
    QRadioButton *radio = qobject_cast<QRadioButton *>(
        _left->itemAt(_selectedLeftPortIndex + 1)->widget());
    if (radio) {
        _radioGroup->removeButton(radio);
    }

    radio->deleteLater();
    delete radio;

    _leftPorts--;
    _letMinusButton->setEnabled(false);
    _model.removeProcessNodePort(_nodeId, PortType::In, _selectedLeftPortIndex);

    adjustSize();
}

void PortAddRemoveWidget::rightMinusClicked()
{
    QRadioButton *radio = qobject_cast<QRadioButton *>(
        _right->itemAt(_selectedRightPortIndex + 1)->widget());
    if (radio) {
        _radioGroup->removeButton(radio);
    }

    radio->deleteLater();
    delete radio;

    _rightPorts--;
    _rightMinusButton->setEnabled(false);
    _model.removeProcessNodePort(_nodeId, PortType::Out, _selectedRightPortIndex);

    adjustSize();
}