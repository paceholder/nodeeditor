#include "DasPwWidget.hpp"
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>

DasPwWidget::DasPwWidget(float &_f, float &_c0, QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    QGridLayout *layout = new QGridLayout(this);

    QLabel *f_label = new QLabel("F#", this);
    layout->addWidget(f_label, 0, 0, 1, 1);
    _f_line = new QLineEdit(this);
    _f_line->setValidator(new QDoubleValidator());
    _f_line->setText(QString::number(_f));
    _f_line->setFixedWidth(40);
    layout->addWidget(_f_line, 0, 1, 1, 1);
    connect(_f_line, &QLineEdit::textChanged, [this, &_f](QString obj) { _f = obj.toFloat(); });

    QLabel *c0_label = new QLabel("c0", this);
    layout->addWidget(c0_label, 1, 0, 1, 1);
    _c0_line = new QLineEdit(this);
    _c0_line->setValidator(new QDoubleValidator());
    _c0_line->setText(QString::number(_c0));
    _c0_line->setFixedWidth(40);
    layout->addWidget(_c0_line, 1, 1, 1, 1);
    connect(_c0_line, &QLineEdit::textChanged, [this, &_c0](QString obj) { _c0 = obj.toFloat(); });
}
