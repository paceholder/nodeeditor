#include "RoiSphereWidget.hpp"
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>

RoiSphereWidget::RoiSphereWidget(float &x, float &y, float &z, float &r, QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    QGridLayout *layout = new QGridLayout(this);

    _x = new QLineEdit(this);
    _x->setValidator(new QDoubleValidator());
    _x->setText(QString::number(x));
    _x->setFixedWidth(40);
    layout->addWidget(_x, 0, 1, 1, 1);
    connect(_x, &QLineEdit::textChanged, [this, &x](QString obj) { x = obj.toFloat(); });

    _y = new QLineEdit(this);
    _y->setValidator(new QDoubleValidator());
    _y->setText(QString::number(y));
    _y->setFixedWidth(40);
    layout->addWidget(_y, 1, 1, 1, 1);
    connect(_y, &QLineEdit::textChanged, [this, &y](QString obj) { y = obj.toFloat(); });

    _z = new QLineEdit(this);
    _z->setValidator(new QDoubleValidator());
    _z->setText(QString::number(z));
    _z->setFixedWidth(40);
    layout->addWidget(_z, 2, 1, 1, 1);
    connect(_z, &QLineEdit::textChanged, [this, &z](QString obj) { z = obj.toFloat(); });

    _r = new QLineEdit(this);
    _r->setValidator(new QDoubleValidator());
    _r->setText(QString::number(r));
    _r->setFixedWidth(40);
    layout->addWidget(_r, 3, 1, 1, 1);
    connect(_r, &QLineEdit::textChanged, [this, &r](QString obj) { r = obj.toFloat(); });

    QLabel *label;
    label = new QLabel("x", this);
    layout->addWidget(label, 0, 0, 1, 1);
    label = new QLabel("y", this);
    layout->addWidget(label, 1, 0, 1, 1);
    label = new QLabel("z", this);
    layout->addWidget(label, 2, 0, 1, 1);
    label = new QLabel("r", this);
    layout->addWidget(label, 3, 0, 1, 1);
}
