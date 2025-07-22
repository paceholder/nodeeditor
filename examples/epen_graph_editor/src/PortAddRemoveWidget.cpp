#include "PortAddRemoveWidget.hpp"

#include "DataFlowModel.hpp"

// Layout constants
static const int BUTTON_HEIGHT = 25;
static const int SPACER_WIDTH = 25;
static const int SPACER_HEIGHT = 22;
static const int RIGHT_SPACER_HEIGHT = 10;

static const int LEFT_SPACING = 13;
static const int RIGHT_SPACING = 13;
static const int CENTER_SPACING = 50;

PortAddRemoveWidget::PortAddRemoveWidget(NodeId nodeId, DataFlowModel &model, QWidget *parent)
    : QWidget(parent)
    , _nodeId(nodeId)
    , _model(model)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);

    _left = new QVBoxLayout();
    _left->setSpacing(LEFT_SPACING);
    _left->setContentsMargins(0, 0, 0, 0);
    _left->addStretch();

    _right = new QVBoxLayout();
    _right->setSpacing(RIGHT_SPACING);
    _right->setContentsMargins(0, 0, 0, 0);
    _right->addStretch();

    hl->addLayout(_left);
    hl->addSpacing(CENTER_SPACING);
    hl->addLayout(_right);

    auto buttonAdd = new QPushButton("+");
    buttonAdd->setFixedHeight(BUTTON_HEIGHT);
    _left->insertWidget(0, buttonAdd);
    connect(buttonAdd, &QPushButton::clicked, this, &PortAddRemoveWidget::addLeftPort);

    // Add empty spacer below + button

    // Spacer for first port (non-deletable)
    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spacer->setFixedSize(SPACER_WIDTH, SPACER_HEIGHT);
    _left->insertWidget(1, spacer);

    auto topButtonsRow = new QHBoxLayout();
    topButtonsRow->setContentsMargins(0, 0, 0, 0);

    _right->insertLayout(0, topButtonsRow);

    auto buttonI = new QPushButton("I");
    buttonI->setFixedHeight(BUTTON_HEIGHT);
    topButtonsRow->addWidget(buttonI);

    auto buttonB = new QPushButton("B");
    buttonB->setFixedHeight(BUTTON_HEIGHT);
    topButtonsRow->addWidget(buttonB);

    // Connect I and B buttons to add right ports
    connect(buttonI, &QPushButton::clicked, this, &PortAddRemoveWidget::addRightPortI);
    connect(buttonB, &QPushButton::clicked, this, &PortAddRemoveWidget::addRightPortB);

    // Add spacer for first right port (non-deletable)
    QWidget *rightSpacer = new QWidget();
    rightSpacer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    rightSpacer->setFixedSize(SPACER_WIDTH, RIGHT_SPACER_HEIGHT);
    _right->insertWidget(1, rightSpacer);
}

void PortAddRemoveWidget::addLeftPort()
{
    auto button = new QPushButton("-");
    button->setFixedHeight(BUTTON_HEIGHT);
    connect(button, &QPushButton::clicked, this, &PortAddRemoveWidget::removeLeftPort);

    // Insert after + button (0), first port spacer (1), and existing ports
    _left->insertWidget(_leftPorts + 2, button);

    // Trigger changes in the model
    _model.addProcessNodePort(_nodeId, PortType::In, _leftPorts, false);
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
    _model.removeProcessNodePort(_nodeId, PortType::In, portIndex);

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

void PortAddRemoveWidget::addRightPort(bool isRight)
{
    auto button = new QPushButton("-");
    button->setFixedHeight(BUTTON_HEIGHT);
    connect(button, &QPushButton::clicked, this, &PortAddRemoveWidget::removeRightPort);

    _right->insertWidget(_rightPorts + 2, button);

    // Trigger changes in the model
    _model.addProcessNodePort(_nodeId, PortType::Out, _rightPorts, isRight);
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