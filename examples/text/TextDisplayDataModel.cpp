#include "TextDisplayDataModel.hpp"

void TextDisplayDataModel::init()
{
    createPort(PortType::In, std::make_shared<TextData>(), "Text");
}

void TextDisplayDataModel::setInData(std::shared_ptr<NodeData> nodeData, PortIndex const)
{
    auto textData = std::dynamic_pointer_cast<TextData>(nodeData);

    _label->setText(textData->data);

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
