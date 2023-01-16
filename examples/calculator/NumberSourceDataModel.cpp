#include "NumberSourceDataModel.hpp"

#include <QtCore/QJsonValue>
#include <QtGui/QDoubleValidator>
#include <QtWidgets/QLineEdit>

NumberSourceDataModel::NumberSourceDataModel()
    : _lineEdit{nullptr}
    , _number(0.0)
{}

void NumberSourceDataModel::init()
{
    createPort(PortType::Out, "decimal", "Decimal", QtNodes::ConnectionPolicy::Many);
}

QJsonObject NumberSourceDataModel::save() const
{
    QJsonObject modelJson;

    modelJson["number"] = QString::number(_number);

    return modelJson;
}

void NumberSourceDataModel::load(QJsonObject const &p)
{
    QJsonValue v = p["number"];

    if (!v.isUndefined()) {
        QString strNum = v.toString();

        bool ok;
        double d = strNum.toDouble(&ok);
        if (ok) {
            _number = d;
            updateOutPortData(0, _number);

            if (_lineEdit)
                _lineEdit->setText(strNum);
        }
    }
}

void NumberSourceDataModel::onTextEdited(QString const &str)
{
    bool ok = false;

    double number = str.toDouble(&ok);

    if (ok) {
        _number = number;
        updateOutPortData(0, _number);

        Q_EMIT dataUpdated(0);

    } else {
        Q_EMIT dataInvalidated(0);
    }
}

QWidget *NumberSourceDataModel::embeddedWidget()
{
    if (!_lineEdit) {
        _lineEdit = new QLineEdit();

        _lineEdit->setValidator(new QDoubleValidator());
        _lineEdit->setMaximumSize(_lineEdit->sizeHint());

        connect(_lineEdit, &QLineEdit::textChanged, this, &NumberSourceDataModel::onTextEdited);

        _lineEdit->setText(QString::number(_number));
    }
    return _lineEdit;
}

void NumberSourceDataModel::setNumber(double n)
{
    _number = n;
    updateOutPortData(0, _number);

    Q_EMIT dataUpdated(0);

    if (_lineEdit)
        _lineEdit->setText(QString::number(_number));
}
