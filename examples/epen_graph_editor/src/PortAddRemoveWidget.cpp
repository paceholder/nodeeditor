#include "PortAddRemoveWidget.hpp"

#include "DataFlowModel.hpp"

// Layout constants
static const int BUTTON_HEIGHT = 25;
static const int SPACER_WIDTH = 25;
static const int SPACER_HEIGHT = 22;
static const int RIGHT_SPACER_HEIGHT = 10;

static const int FIRST_NODE_SPACING = 21;
static const int NODE_SPACING = 13;
static const int CENTER_SPACING = 50;

PortAddRemoveWidget::PortAddRemoveWidget(NodeId nodeId, DataFlowModel &model, QWidget *parent)
    : QWidget(parent)
    , _nodeId(nodeId)
    , _model(model)
{
    // Widget constructor with proper top-alignment
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setContentsMargins(0, 0, 0, 0);
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

    auto buttonILeft = new QPushButton("I");
    buttonILeft->setFixedHeight(BUTTON_HEIGHT);
    topButtonsLeftRow->addWidget(buttonILeft);

    auto buttonBLeft = new QPushButton("B");
    buttonBLeft->setFixedHeight(BUTTON_HEIGHT);
    topButtonsLeftRow->addWidget(buttonBLeft);

    // Add the button layout to the top of left side
    _left->addLayout(topButtonsLeftRow);

    // Add 13-pixel spacer after buttons
    _left->addSpacing(FIRST_NODE_SPACING);

    // Connect left buttons
    connect(buttonBLeft, &QPushButton::clicked, this, &PortAddRemoveWidget::addLeftPortB);
    connect(buttonILeft, &QPushButton::clicked, this, &PortAddRemoveWidget::addLeftPortI);

    // RIGHT SIDE: Add I and B buttons
    auto topButtonsRightRow = new QHBoxLayout();
    topButtonsRightRow->setContentsMargins(0, 0, 0, 0);
    topButtonsRightRow->setSpacing(4); // Add small spacing between buttons

    auto buttonI = new QPushButton("I");
    buttonI->setFixedHeight(BUTTON_HEIGHT);
    topButtonsRightRow->addWidget(buttonI);

    auto buttonB = new QPushButton("B");
    buttonB->setFixedHeight(BUTTON_HEIGHT);
    topButtonsRightRow->addWidget(buttonB);

    // Add the button layout to the top of right side
    _right->addLayout(topButtonsRightRow);

    // Add 13-pixel spacer after buttons
    _right->addSpacing(FIRST_NODE_SPACING);

    // Connect right buttons
    connect(buttonI, &QPushButton::clicked, this, &PortAddRemoveWidget::addRightPortI);
    connect(buttonB, &QPushButton::clicked, this, &PortAddRemoveWidget::addRightPortB);

    // Add stretch at the END to push everything to the top
    _left->addStretch();
    _right->addStretch();

    // Alternative approach if you need more control over the spacer
}

void PortAddRemoveWidget::addLeftPort(bool isImage)
{
    auto button = new QPushButton("-");
    button->setFixedHeight(BUTTON_HEIGHT);
    connect(button, &QPushButton::clicked, this, &PortAddRemoveWidget::removeLeftPort);

    // Insert after + button (0), first port spacer (1), and existing ports
    _left->insertWidget(_leftPorts + 2, button);

    // Trigger changes in the model
    _model.addProcessNodePort(_nodeId, PortType::In, _leftPorts, isImage);
    _leftPorts++;

    adjustSize();
}

void PortAddRemoveWidget::removeLeftPort()
{
    // Find which button was clicked
    QPushButton *clickedButton = qobject_cast<QPushButton *>(QObject::sender());
    if (!clickedButton)
        return;

    // Find the index of the clicked button in the left layout
    int buttonIndex = -1;
    for (int i = 2; i < _left->count(); ++i) {
        QLayoutItem *item = _left->itemAt(i);
        if (item && item->widget() == clickedButton) {
            buttonIndex = i;
            break;
        }
    }

    if (buttonIndex == -1)
        return;

    // Calculate the port index (subtract 3 for + button, empty spacer, and first port spacer)
    PortIndex portIndex = buttonIndex - 3;

    // Remove the button
    auto item = _left->takeAt(buttonIndex);
    if (item) {
        item->widget()->deleteLater();
        delete item;
    }

    _leftPorts--;
    // Trigger changes in the model
    _model.removeProcessNodePort(_nodeId, PortType::In, buttonIndex - 1);

    adjustSize();
}

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
    auto button = new QPushButton("-");
    button->setFixedHeight(BUTTON_HEIGHT);
    connect(button, &QPushButton::clicked, this, &PortAddRemoveWidget::removeRightPort);

    _right->insertWidget(_rightPorts + 2, button);

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

    // Find the index of the clicked button in the right layout
    int buttonIndex = -1;
    for (int i = 2; i < _right->count();
         ++i) { // Start from 2 to skip the top buttons row and spacer
        QLayoutItem *item = _right->itemAt(i);
        if (item && item->widget() == clickedButton) {
            buttonIndex = i;
            break;
        }
    }

    if (buttonIndex == -1)
        return;

    // Calculate the port index (subtract 2 for the top buttons row and spacer)
    PortIndex portIndex = buttonIndex - 2;

    // Remove the button
    auto item = _right->takeAt(buttonIndex);
    if (item) {
        item->widget()->deleteLater();
        delete item;
    }

    _rightPorts--;

    // Trigger changes in the model
    _model.removeProcessNodePort(_nodeId, PortType::Out, buttonIndex - 1);

    adjustSize();
}

PortAddRemoveWidget::~PortAddRemoveWidget()
{
    //
}

std::pair<PortType, PortIndex> PortAddRemoveWidget::findWhichPortWasClicked(QObject *sender,
                                                                            int const buttonIndex)
{
    PortType portType = PortType::None;
    PortIndex portIndex = QtNodes::InvalidPortIndex;

    auto checkOneSide = [&portType, &portIndex, &sender, &buttonIndex](QVBoxLayout *sideLayout) {
        for (int i = 0; i < sideLayout->count(); ++i) {
            auto layoutItem = sideLayout->itemAt(i);
            auto hLayout = dynamic_cast<QHBoxLayout *>(layoutItem);

            if (!hLayout)
                continue;

            auto widget = static_cast<QWidgetItem *>(hLayout->itemAt(buttonIndex))->widget();

            if (sender == widget) {
                portIndex = i;
                break;
            }
        }
    };

    checkOneSide(_left);

    if (portIndex != QtNodes::InvalidPortIndex) {
        portType = PortType::In;
    } else {
        checkOneSide(_right);

        if (portIndex != QtNodes::InvalidPortIndex) {
            portType = PortType::Out;
        }
    }

    return std::make_pair(portType, portIndex);
}