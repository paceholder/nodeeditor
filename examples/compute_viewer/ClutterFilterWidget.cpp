#include "ClutterFilterWidget.hpp"
#include <algorithm>
#include <cmath>
#include <QDoubleValidator>
#include <QGridLayout>
#include <QLineEdit>
#include <QSizePolicy>
#include <QSlider>
#include <Qt>

ClutterFilterWidget::ClutterFilterWidget(float &_value, QWidget *parent)
    : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Policy::Minimum, QSizePolicy::Policy::Minimum);

    QGridLayout *layout = new QGridLayout(this);

    _value = std::clamp(_value, 0.15f, 0.75f);

    _value_line = new QLineEdit(this);
    _value_line->setValidator(new QDoubleValidator());
    _value_line->setText(QString::number(_value));
    _value_line->setFixedWidth(40);
    layout->addWidget(_value_line, 0, 0, 1, 1);

    _value_scroll = new QSlider(Qt::Orientation::Horizontal, this);
    _value_scroll->setMinimum(15);
    _value_scroll->setMaximum(75);
    _value_scroll->setValue(round(_value * 100));
    _value_scroll->setFixedWidth(150);
    layout->addWidget(_value_scroll, 0, 1, 1, 1);

    connect(_value_line, &QLineEdit::textChanged, [this, &_value](QString obj) {
        _value = obj.toFloat();
        _value_scroll->setValue(static_cast<int>(round(_value * 100.)));
    });
    connect(_value_scroll, &QSlider::valueChanged, [this, &_value](int value) {
        _value = static_cast<float>(value) / 100.;
        _value_line->setText(QString::number(_value));
    });
}
