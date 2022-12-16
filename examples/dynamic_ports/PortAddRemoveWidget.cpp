#include "PortAddRemoveWidget.hpp"

#include "DynamicPortsModel.hpp"

PortAddRemoveWidget::PortAddRemoveWidget(unsigned int nInPorts,
                                         unsigned int nOutPorts,
                                         NodeId nodeId,
                                         DynamicPortsModel &model,
                                         QWidget *parent)
    : QWidget(parent)
    , _nodeId(nodeId)
    , _model(model)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    QHBoxLayout *hl = new QHBoxLayout(this);
    hl->setContentsMargins(0, 0, 0, 0);
    hl->setSpacing(0);

    _left = new QVBoxLayout();
    _left->setSpacing(0);
    _left->setContentsMargins(0, 0, 0, 0);
    _left->addStretch();

    _right = new QVBoxLayout();
    _right->setSpacing(0);
    _right->setContentsMargins(0, 0, 0, 0);
    _right->addStretch();

    hl->addLayout(_left);
    hl->addSpacing(50);
    hl->addLayout(_right);
}

PortAddRemoveWidget::~PortAddRemoveWidget()
{
    //
}

void PortAddRemoveWidget::populateButtons(PortType portType, unsigned int nPorts)
{
    QVBoxLayout *vl = (portType == PortType::In) ? _left : _right;

    // we use [-1} in the expression `vl->count() - 1` because
    // one element - a spacer - is alvays present in this layout.

    if (vl->count() - 1 < nPorts)
        while (vl->count() - 1 < nPorts) {
            addButtonGroupToLayout(vl, 0);
        }

    if (vl->count() - 1 > nPorts) {
        while (vl->count() - 1 > nPorts) {
            removeButtonGroupFromLayout(vl, 0);
        }
    }
}

QHBoxLayout *PortAddRemoveWidget::addButtonGroupToLayout(QVBoxLayout *vbl, unsigned int portIndex)
{
    auto l = new QHBoxLayout();
    l->setContentsMargins(0, 0, 0, 0);

    auto button = new QPushButton("+");
    button->setFixedHeight(25);
    l->addWidget(button);
    connect(button, &QPushButton::clicked, this, &PortAddRemoveWidget::onPlusClicked);

    button = new QPushButton("-");
    button->setFixedHeight(25);
    l->addWidget(button);
    connect(button, &QPushButton::clicked, this, &PortAddRemoveWidget::onMinusClicked);

    vbl->insertLayout(portIndex, l);

    return l;
}

void PortAddRemoveWidget::removeButtonGroupFromLayout(QVBoxLayout *vbl, unsigned int portIndex)
{
    // Last item in the layout is always a spacer
    if (vbl->count() > 1) {
        auto item = vbl->itemAt(portIndex);

        // Delete [+] and [-] QPushButton widgets
        item->layout()->itemAt(0)->widget()->deleteLater();
        item->layout()->itemAt(1)->widget()->deleteLater();

        vbl->removeItem(item);

        delete item;
    }
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
    _model.addPort(_nodeId, portType, portIndex + 1);

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
    _model.removePort(_nodeId, portType, portIndex);

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
