#include "TextSourceDataModel.hpp"

void TextSourceDataModel::init()
{
    _result = std::make_shared<TextData>();
    createPort(PortType::Out, _result, "Text", QtNodes::ConnectionPolicy::Many);
}
QWidget *TextSourceDataModel::embeddedWidget()
{
    if (!_lineEdit) {
        _lineEdit = new QLineEdit("Default Text"),

        connect(_lineEdit, &QLineEdit::textEdited, this, &TextSourceDataModel::onTextEdited);

        _result->data = _lineEdit->text();
    }
    return _lineEdit;
}

void TextSourceDataModel::onTextEdited(QString const &string)
{
    _result->data = string;

    Q_EMIT dataUpdated(0);
}
