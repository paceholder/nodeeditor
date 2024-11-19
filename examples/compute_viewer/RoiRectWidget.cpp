#include "RoiRectWidget.hpp"
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QSizePolicy>

RoiRectWidget::RoiRectWidget(
    float &x1, float &x2, float &y1, float &y2, float &z1, float &z2, QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    QGridLayout *layout = new QGridLayout(this);

    _x1 = new QLineEdit(this);
    _x1->setValidator(new QDoubleValidator());
    _x1->setText(QString::number(x1));
    _x1->setFixedWidth(40);
    layout->addWidget(_x1, 1, 1, 1, 1);
    connect(_x1, &QLineEdit::textChanged, [this, &x1](QString obj) { x1 = obj.toFloat(); });

    _x2 = new QLineEdit(this);
    _x2->setValidator(new QDoubleValidator());
    _x2->setText(QString::number(x2));
    _x2->setFixedWidth(40);
    layout->addWidget(_x2, 1, 2, 1, 1);
    connect(_x2, &QLineEdit::textChanged, [this, &x2](QString obj) { x2 = obj.toFloat(); });

    _y1 = new QLineEdit(this);
    _y1->setValidator(new QDoubleValidator());
    _y1->setText(QString::number(y1));
    _y1->setFixedWidth(40);
    layout->addWidget(_y1, 2, 1, 1, 1);
    connect(_y1, &QLineEdit::textChanged, [this, &y1](QString obj) { y1 = obj.toFloat(); });

    _y2 = new QLineEdit(this);
    _y2->setValidator(new QDoubleValidator());
    _y2->setText(QString::number(y2));
    _y2->setFixedWidth(40);
    layout->addWidget(_y2, 2, 2, 1, 1);
    connect(_y2, &QLineEdit::textChanged, [this, &y2](QString obj) { y2 = obj.toFloat(); });

    _z1 = new QLineEdit(this);
    _z1->setValidator(new QDoubleValidator());
    _z1->setText(QString::number(z1));
    _z1->setFixedWidth(40);
    layout->addWidget(_z1, 3, 1, 1, 1);
    connect(_z1, &QLineEdit::textChanged, [this, &z1](QString obj) { z1 = obj.toFloat(); });

    _z2 = new QLineEdit(this);
    _z2->setValidator(new QDoubleValidator());
    _z2->setText(QString::number(z2));
    _z2->setFixedWidth(40);
    layout->addWidget(_z2, 3, 2, 1, 1);
    connect(_z2, &QLineEdit::textChanged, [this, &z2](QString obj) { z2 = obj.toFloat(); });

    QLabel *label;
    label = new QLabel("x", this);
    layout->addWidget(label, 1, 0, 1, 1);
    label = new QLabel("y", this);
    layout->addWidget(label, 2, 0, 1, 1);
    label = new QLabel("z", this);
    layout->addWidget(label, 3, 0, 1, 1);

    label = new QLabel("min", this);
    layout->addWidget(label, 0, 1, 1, 1);
    label = new QLabel("max", this);
    layout->addWidget(label, 0, 2, 1, 1);
}
