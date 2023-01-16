#include "TextSourceDataModel.hpp"

#include <QtWidgets/QLineEdit>

void TextSourceDataModel::init()
{
    createPort(PortType::Out, "text", "Text", QtNodes::ConnectionPolicy::Many);
}

QWidget *TextSourceDataModel::embeddedWidget()
{
    if (!_lineEdit) {
        _lineEdit = new QLineEdit("Default Text"),

        connect(_lineEdit, &QLineEdit::textEdited, this, &TextSourceDataModel::onTextEdited);

        updateOutPortData(0, _lineEdit->text());
    }
    return _lineEdit;
}

void TextSourceDataModel::onTextEdited(QString const &string)
{
    updateOutPortData(0, string);

    Q_EMIT dataUpdated(0);
}
