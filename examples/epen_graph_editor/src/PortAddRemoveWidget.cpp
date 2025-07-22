#include "PortAddRemoveWidget.hpp"

#include "DataFlowModel.hpp"

PortAddRemoveWidget::PortAddRemoveWidget(NodeId nodeId, DataFlowModel &model, QWidget *parent)
    : QWidget(parent)
    , _nodeId(nodeId)
    , _model(model)
{
    // 10 + 45 * i
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Expanding);

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);

    _left = new QVBoxLayout();
    _left->setSpacing(13);
    _left->setContentsMargins(0, 0, 0, 0);
    _left->addStretch();

    _right = new QVBoxLayout();
    _right->setSpacing(0);
    _right->setContentsMargins(0, 0, 0, 0);
    _right->addStretch();

    hl->addLayout(_left);
    hl->addSpacing(50);
    hl->addLayout(_right);

    auto buttonAdd = new QPushButton("+");
    buttonAdd->setFixedHeight(25);
    _left->insertWidget(0, buttonAdd);

    QWidget *spacer = new QWidget();
    spacer->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
    spacer->setFixedSize(50, 23);
    _left->insertWidget(1, spacer);
    connect(buttonAdd, &QPushButton::clicked, this, &PortAddRemoveWidget::addLeftPort);

    auto topButtonsRow = new QHBoxLayout();
    topButtonsRow->setContentsMargins(0, 0, 0, 0);

    _right->insertLayout(0, topButtonsRow);

    auto buttonI = new QPushButton("I");
    buttonI->setFixedHeight(25);
    topButtonsRow->addWidget(buttonI);

    auto buttonB = new QPushButton("B");
    buttonB->setFixedHeight(25);
    topButtonsRow->addWidget(buttonB);
    //connect(buttonI, &QPushButton::clicked, this, &PortAddRemoveWidget::onPlusClicked);
}

void PortAddRemoveWidget::addLeftPort()
{
    //addButtonGroupToLayout((portType == PortType::In) ? _left : _right, portIndex + 1);

    auto button = new QPushButton("-");
    button->setFixedHeight(25);
    connect(button, &QPushButton::clicked, this, &PortAddRemoveWidget::removeLeftPort);

    _left->insertWidget(_leftPorts + 1, button);

    // Trigger changes in the model
    _model.addProcessNodePort(_nodeId, PortType::In, _leftPorts);
    _leftPorts++;

    adjustSize();
}

void PortAddRemoveWidget::removeLeftPort()
{
    int const minusButtonIndex = 1;

    PortType portType;
    PortIndex portIndex;

    std::tie(portType, portIndex) = findWhichPortWasClicked(QObject::sender(), minusButtonIndex);

    //removeButtonGroupFromLayout(_left, portIndex);

    auto item = _left->itemAt(minusButtonIndex + 1);

    item->widget()->deleteLater();

    _left->removeItem(item);

    delete item;

    _leftPorts--;
    // Trigger changes in the model
    _model.removeProcessNodePort(_nodeId, PortType::In, portIndex);

    adjustSize();
}

PortAddRemoveWidget::~PortAddRemoveWidget()
{
    //
}

void PortAddRemoveWidget::populateButtons(PortType portType, unsigned int nPorts)
{
    /*QVBoxLayout *vl = (portType == PortType::In) ? _left : _right;

    if (vl->count() - 1 < nPorts)
        while (vl->count() - 1 < nPorts) {
            addButtonGroupToLayout(vl, 0);
        }

    if (vl->count() - 1 > nPorts) {
        while (vl->count() - 1 > nPorts) {
            removeButtonGroupFromLayout(vl, 0);
        }
    }*/
}

QHBoxLayout *PortAddRemoveWidget::addButtonGroupToLayout(QVBoxLayout *vbl, unsigned int portIndex)
{
    auto l = new QHBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);

    /*auto button = new QPushButton("+");
    button->setFixedHeight(25);
    l->addWidget(button);
    connect(button, &QPushButton::clicked, this, &PortAddRemoveWidget::onPlusClicked);*/

    auto button = new QPushButton("-");
    button->setFixedHeight(25);
    l->addWidget(button);
    connect(button, &QPushButton::clicked, this, &PortAddRemoveWidget::onMinusClicked);

    vbl->insertLayout(portIndex, l);

    return l;
}

void PortAddRemoveWidget::removeButtonGroupFromLayout(QVBoxLayout *vbl, unsigned int portIndex)
{
    // Last item in the layout is always a spacer
    /*if (vbl->count() > 1) {
        auto item = vbl->itemAt(portIndex);

        // Delete [+] and [-] QPushButton widgets
        item->layout()->itemAt(0)->widget()->deleteLater();
        item->layout()->itemAt(1)->widget()->deleteLater();

        vbl->removeItem(item);

        delete item;
    }*/
}

void PortAddRemoveWidget::onPlusClicked()
{
    // index of the plus button in the QHBoxLayout
    int const plusButtonIndex = 0;

    PortType portType;
    PortIndex portIndex;

    // All existing "plus" buttons trigger the same slot. We need to find out which
    // button has been actually clicked.
    std::tie(portType, portIndex) = findWhichPortWasClicked(QObject::sender(), plusButtonIndex);

    // We add new "plus-minus" button group to the chosen layout.
    addButtonGroupToLayout((portType == PortType::In) ? _left : _right, portIndex + 1);

    // Trigger changes in the model
    _model.addProcessNodePort(_nodeId, portType, portIndex + 1);

    adjustSize();
}

void PortAddRemoveWidget::onMinusClicked()
{
    // index of the minus button in the QHBoxLayout
    int const minusButtonIndex = 1;

    PortType portType;
    PortIndex portIndex;

    std::tie(portType, portIndex) = findWhichPortWasClicked(QObject::sender(), minusButtonIndex);

    removeButtonGroupFromLayout((portType == PortType::In) ? _left : _right, portIndex);

    // Trigger changes in the model
    _model.removeProcessNodePort(_nodeId, portType, portIndex);

    adjustSize();
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
