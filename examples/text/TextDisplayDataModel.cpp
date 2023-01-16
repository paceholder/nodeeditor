#include "TextDisplayDataModel.hpp"

#include <QtWidgets/QLabel>

void TextDisplayDataModel::init()
{
    createPort(PortType::In, "text", "Text");
}

void TextDisplayDataModel::setInData(QVariant const nodeData, PortIndex const)
{
    if (!nodeData.isNull()) {
        _label->setText(nodeData.toString());
    } else {
        _label->clear();
    }

    _label->adjustSize();
}

QWidget *TextDisplayDataModel::embeddedWidget()
{
    if (!_label) {
        _label = new QLabel("Resulting Text");

        _label->setMargin(3);
    }
    return _label;
}
